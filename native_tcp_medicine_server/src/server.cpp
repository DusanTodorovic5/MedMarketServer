#include "../hpp/server.hpp"

Server::Server(int port, std::string interface) {
    MedLogger::init(new ConsoleMedLogger());

    MedLogger::instance()->setName("Server");

    vault = new UserVault();
    market = new MedicineMarket();
    
    nt_socket.Create(true, false, false);

    nt_socket.Bind(port, interface.c_str());
}


void Server::run() {
    nt_socket.Listen(SOMAXCONN);

    MedLogger::instance()->log("Started listening: ");
    
    while (1) {
        SOCKET out;
        sockaddr_in client;
        nt_socket.Accept(out, &client);

        Client* newClient = new Client(out, client, this->market, this->vault);

        std::thread th(*newClient);

        clients.push_back(newClient);
        th_clients.push_back(std::move(th));
    }
}

Server::~Server() {
    nt_socket.Close();

    for (int i = 0; i < th_clients.size(); i++) {
        th_clients[i].join();
    }

    for (Client* c : clients) {
        delete c;
    }

    delete market;
    delete vault;
    MedLogger::finish();
}