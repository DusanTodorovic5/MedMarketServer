#include "../hpp/user_vault.hpp"
#include <fstream>
#include <sys/stat.h>
#include <vector>
#include <sstream>
#include <iostream>


long long UserVault::currToken = 1;

UserVault::UserVault() : running(true), thr([this]{this->checkTimeout();}) {
    struct stat buffer;
    if (stat ("users", &buffer) != 0) return;

    std::fstream users;
    users.open("users",std::ios::in);
    std::string line;
    std::string part;
    while(getline(users, line)){
        std::vector<std::string> splitted;
        std::stringstream s(line);

        while (getline(s, part, ';')) {
            splitted.push_back(part);
        }

        size_t pass;
        sscanf(splitted[1].c_str(), "%zu", &pass);
        this->map[splitted[0]] = pass;
    }

    users.close();
}

UserVault::~UserVault() {
    running = false;
}

std::string UserVault::signin(std::string username, std::string password) {
    size_t pass = hasher(password);
    if (pass == this->map[username]) {
        mutex.lock();
        size_t token = generateId(username + password);
        currentUsers[token] = User(username, std::time(0));
        mutex.unlock();
        return std::to_string(token);
    }
    return "";
}

size_t UserVault::generateId(std::string id) {
    id += std::to_string(++currToken);
    return hasher(id);
}

bool UserVault::checkAuth(std::string username, std::string password) {
    size_t pass = hasher(password);
    return (pass == this->map[username]);
}

bool UserVault::signup(std::string username, std::string password) {
    mutex.lock();
    if (map.count(username) != 0) {
        mutex.unlock();
        return false;
    }
    size_t pass = hasher(password);
    this->map[username] = pass;
    persist(username, password);
    mutex.unlock();
    return true;
}

bool UserVault::update(std::string username, std::string oldPassword, std::string newPassword) {
    mutex.lock();
    if (!checkAuth(username, oldPassword)) {
        mutex.unlock();
        return false;
    }

    std::ofstream outfile;

    outfile.open("users");
    
    std::unordered_map<std::string, size_t>::iterator it;
    for (it = map.begin(); it != map.end(); it++) {
        outfile << (*it).first << (*it).second << ";" << "\n";
    }

    outfile.flush();
    outfile.close();
    mutex.unlock();
    return true;
}

void UserVault::persist(std::string username, std::string password) {
    std::ofstream outfile;

    outfile.open("users", std::ios_base::app);
    outfile << username << ";" << hasher(password) << "\n";

    outfile.flush();
    outfile.close();
}

void UserVault::checkTimeout() {
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::vector<size_t> forDeletion;
        std::unordered_map<size_t, User>::iterator it;
        mutex.lock();
        for (it = currentUsers.begin(); it != currentUsers.end(); it++) {
            if (std::time(0) - it->second.last > 30) { // 30 seconds
                forDeletion.push_back(it->first);
            }
        }

        for (size_t st : forDeletion) {
            currentUsers.erase(st);
        }

        mutex.unlock();
    }
}

std::pair<bool, std::string> UserVault::checkToken(std::string token) {
    std::stringstream sstream(token);
    size_t result;
    sstream >> result;
    mutex.lock();
    if (currentUsers.find(result) == currentUsers.end()) {
        mutex.unlock();
        return {false, "Failed to Authenticate!"};
    }
    std::string username = currentUsers[result].username;
    currentUsers[result].last = std::time(0);
    mutex.unlock();
    return {true, username};
}