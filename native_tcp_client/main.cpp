#include "ntsocket.h"
#include <iostream>

int main(int argc, char const* argv[])
{
    NTSocket clientSocket(SocketCreateType::SCT_CLIENT_CONNECT);

    clientSocket.Create(true, false, false);

    in_addr inaddr;

    inet_aton("192.168.0.200", &inaddr);

    clientSocket.Connect(inaddr.s_addr, 54001);

    int valread;
    char buffer[4096];

    std::cout << "Enter header: " << std::endl;

    std::string header;
    std::getline(std::cin, header);

    clientSocket.AggressiveBlockedSend(header.data(), header.size());

    valread = clientSocket.AggressiveBlockedRead(buffer, 4096);

    printf("%s\n", buffer);

    if (buffer[0] == 'F') return -1;

    std::cout << "Enter query: " << std::endl;

    std::string query;
    std::getline(std::cin, query);

    if (query.length() == 0) { return -2; }

    clientSocket.AggressiveBlockedSend(query.data(), query.size());

    valread = clientSocket.AggressiveBlockedRead(buffer, 4096);

    printf("%s\n", buffer);

    clientSocket.Close();

    return 0;
}
