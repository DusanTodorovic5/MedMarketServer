#ifndef CLIENT_CLASS
#define CLIENT_CLASS

#include "medicine_market.hpp"
#include "../ntsocket.h"
#include "message.hpp"
#include "user_vault.hpp"

class Client {
    public:
        Client(SOCKET, sockaddr_in, MedicineMarket*, UserVault*);
        ~Client();
        void operator()();
    private:
        bool checkAuth(char* buf);
        bool trySignin(std::string, std::string);
        bool trySignup(std::string, std::string);
        bool tryUpdate(std::string, std::string, std::string);
        NTSocket* nt_socket;
        MedicineMarket* market;
        UserVault* vault;
        sockaddr_in client_addr;
        SOCKET client_socket;
};

#endif