#ifndef SERVER_CLASS
#define SERVER_CLASS

#include "../hpp/client.hpp"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>

#include <iostream>
#include <set>
#include <unordered_map>

#include "user_vault.hpp"
#include "medicine_market.hpp"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using websocketpp::lib::thread;
using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

class Server {
    public:
        Server();
        void run(uint16_t port);
        void on_open(connection_hdl hdl);
        void on_message(connection_hdl hdl, server::message_ptr msg);
        void on_close(connection_hdl hdl);
        void on_fail(connection_hdl hdl);
        void broadcast();
        ~Server();
    private:
        std::vector<std::string> split(std::string s);
        bool checkAuth(std::string header, connection_hdl hdl);
        bool trySignin(std::string username, std::string password, connection_hdl hdl);
        bool trySignup(std::string username, std::string password, connection_hdl hdl);
        bool tryUpdate(std::string username, std::string oldPassword, std::string newPassword, connection_hdl hdl);
        Client* getClient(connection_hdl hdl);

        typedef std::map<std::string, connection_hdl> con_list;

        server m_server;
        std::vector<Client*> connections;
        con_list m_connections;
        mutex m_action_lock;
        UserVault* vault;
        MedicineMarket* market;
        thread thr;
        bool running;
};

#endif