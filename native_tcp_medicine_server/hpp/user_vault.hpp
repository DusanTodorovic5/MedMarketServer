#ifndef USER_VAULT
#define USER_VAULT

#include <unordered_map>
#include <string>
#include <mutex>

class UserVault {
    public:
        UserVault();
        bool signin(std::string, std::string);
        bool signup(std::string, std::string);
        bool update(std::string, std::string, std::string);
    private:
        void persist(std::string, std::string);
        std::unordered_map<std::string, size_t> map;
        std::hash<std::string> hasher;
        std::mutex mutex;
};

#endif