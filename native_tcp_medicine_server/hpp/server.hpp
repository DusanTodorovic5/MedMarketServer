#ifndef SERVER_CLASS
#define SERVER_CLASS

#include "../ntsocket.h"
#include "client.hpp"
#include <thread>
#include <iostream>
#include <vector>
#include <thread>
#include "med_console_logger.hpp"
#include "med_logger.hpp"
#include "medicine_market.hpp"
#include "user_vault.hpp"

class Server {
    public:
        Server(int port = 54001, std::string interface = "192.168.0.200");
        void run();
        ~Server();
    private:
        NTSocket nt_socket; 
        MedicineMarket* market;
        std::vector<std::thread> th_clients;
        std::vector<Client*> clients;
        UserVault* vault;
};

#endif