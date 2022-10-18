#ifndef USER_VAULT
#define USER_VAULT

#include <unordered_map>
#include <string>
#include <mutex>
#include <thread>
#include <chrono>

struct User {
    std::string username;
    std::time_t last;

    User(std::string username = "", std::time_t time = 0) {
        this->username = username;
        this->last = time;
    }
};

class UserVault {
    public:
        UserVault();
        ~UserVault();
        std::pair<bool, std::string> checkToken(std::string);
        std::string signin(std::string, std::string);
        bool signup(std::string, std::string);
        bool update(std::string, std::string, std::string);
        bool checkAuth(std::string, std::string);
        size_t generateId(std::string);
        void checkTimeout();
        
    private:
        void persist(std::string, std::string);
        std::unordered_map<std::string, size_t> map;
        std::hash<std::string> hasher;
        std::unordered_map<size_t, User> currentUsers;
        std::mutex mutex;
        static long long currToken;
        bool running;
        std::thread thr;
};

#endif