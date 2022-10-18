#ifndef SERVER_CLASS
#define SERVER_CLASS

#include "medicine_market.hpp"
#include "../httplib.h"
#include "user_vault.hpp"

class MedServer {
    public:
        MedServer();
        ~MedServer();
        void listen();
        std::string signin(const httplib::Request& req);
        std::pair<bool, std::string> checkToken(const httplib::Request& req);
        bool signup(std::string, std::string);
        MedicineMarket* getMarket();
        std::vector<std::string> split(std::string s, std::string delimeter = ",");
    private:
        MedicineMarket* market;
        httplib::Server server;
        UserVault vault;
};

#endif