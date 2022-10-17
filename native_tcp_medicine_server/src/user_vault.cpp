#include "../hpp/user_vault.hpp"
#include <fstream>
#include <sys/stat.h>
#include <vector>
#include <sstream>

UserVault::UserVault() {
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

bool UserVault::signin(std::string username, std::string password) {
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
    if (!signin(username, oldPassword)) {
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