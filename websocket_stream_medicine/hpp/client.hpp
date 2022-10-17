#ifndef CLIENT_STRUCT
#define CLIENT_STRUCT

#include <websocketpp/server.hpp>

struct Client {
    Client(std::string username, websocketpp::connection_hdl hdl) : username(username), hdl(hdl) { } 

    websocketpp::connection_hdl hdl;
    std::string username;
};

#endif