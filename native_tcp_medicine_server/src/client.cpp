#include "../hpp/client.hpp"
#include <iostream>
#include "../hpp/med_logger.hpp"

Client::Client(SOCKET socket, sockaddr_in addr, MedicineMarket* market, UserVault* vault)
 : client_socket(socket), client_addr(addr) {
    this->market = market;
    this->vault = vault;

    nt_socket = new NTSocket(SocketCreateType::SCT_CLIENT_CONNECT, socket);
}

Client::~Client() {
   delete nt_socket;
}

void Client::operator()() {

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
 
    memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    if (getnameinfo((sockaddr*)&client_addr, sizeof(client_addr), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        std::string str(service);
        MedLogger::instance()->setName(str);
        MedLogger::instance()->log(std::string(host) + " connected on port " + service);
        
    }
    else
    {
        MedLogger::instance()->setName(std::to_string(ntohs(client_addr.sin_port)));
        inet_ntop(AF_INET, &client_addr.sin_addr, host, NI_MAXHOST);
        MedLogger::instance()->log(std::string(host) + " connected on port " + std::to_string(ntohs(client_addr.sin_port)));
    }

    char buf[4096];

    if (!checkAuth(buf)) {
        return;
    }
    
    memset(buf, 0, 4096);

    while (true)
    {
        Message msg;
        int bytesReceived = nt_socket->AggressiveBlockedRead(buf,4096);
        if (bytesReceived == -1)
        {
            MedLogger::instance()->error("Error in recv(). Quitting");
            break;
        }

        if (bytesReceived == 0)
        {
            MedLogger::instance()->warning("Client disconnected");
            break;
        }
 
        std::string query = std::string(buf, 0, bytesReceived);
        
        market->query(query, msg);

        nt_socket->AggressiveBlockedSend(msg.str(), msg.size());
    }

    nt_socket->Close();
}

std::vector<std::string> split(std::string s) {
    std::string delimiter = ";";
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        if (token != "") res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

bool Client::checkAuth(char* buf) {
    memset(buf, 0, 4096);
    int bytesReceived = nt_socket->AggressiveBlockedRead(buf,4096);
    if (bytesReceived == -1)
    {
        MedLogger::instance()->error("Error in recv(). Quitting");
        return false;
    }

    if (bytesReceived == 0)
    {
        MedLogger::instance()->error("Client disconnected");
        return false;
    }

    std::string header = std::string(buf, 0, bytesReceived);

    std::vector<std::string> headerVector = split(header);

    if (headerVector[0] == "login" && headerVector.size() == 3) {
        return trySignin(headerVector[1], headerVector[2]);
    }

    if (headerVector[0] == "register" && headerVector.size() == 3) {
        return trySignup(headerVector[1], headerVector[2]);
    }

    if (headerVector[0] == "update" && headerVector.size() == 4) {
        return tryUpdate(headerVector[1], headerVector[2], headerVector[3]);
    }

    nt_socket->AggressiveBlockedSend("Failed to authenticate. Bad Header. Closing...\n", 48);
    nt_socket->Close();
    MedLogger::instance()->error("Received uknown header!");
    return false;
}

bool Client::trySignin(std::string username, std::string password) {
    if (vault->signin(username, password)) {
        nt_socket->AggressiveBlockedSend("Authenticated", 14);
        MedLogger::instance()->setName(username);
        return true;
    }

    nt_socket->AggressiveBlockedSend("Failed to authenticate. Closing...\n", 36);
    nt_socket->Close();
    MedLogger::instance()->error("Failed to authenticated!");
    return false;
}

bool Client::trySignup(std::string username, std::string password) {
    if (vault->signup(username, password)) {
        nt_socket->AggressiveBlockedSend("Signed up and logged in", 25);
        MedLogger::instance()->setName(username);
        return true;
    }

    nt_socket->AggressiveBlockedSend("Failed to sign up. Closing...\n", 31);
    nt_socket->Close();
    MedLogger::instance()->error("Failed to sign in!");
    return false;
}

bool Client::tryUpdate(std::string username, std::string oldPassword, std::string newPassword) {
    if (vault->update(username, oldPassword, newPassword)) {
        nt_socket->AggressiveBlockedSend("Password updated and logged in", 31);
        MedLogger::instance()->setName(username);
        return true;
    }

    nt_socket->AggressiveBlockedSend("Failed to update password. Closing...\n", 39);
    nt_socket->Close();
    MedLogger::instance()->error("failed to update password!");
    return false;
}