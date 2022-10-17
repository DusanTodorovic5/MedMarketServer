#include "../hpp/server.hpp"
#include "../hpp/med_logger.hpp"
#include "../hpp/med_console_logger.hpp"
#include "../hpp/message.hpp"
#include <algorithm>
#include <chrono>

Server::Server() : running(true), thr(bind(&Server::broadcast, this)) {
    
    vault = new UserVault();
    market = new MedicineMarket();

    m_server.init_asio();

    m_server.set_open_handshake_timeout(50000);
    m_server.set_pong_timeout(50000);
    m_server.set_close_handshake_timeout(50000);

    m_server.set_open_handler(bind(&Server::on_open, this, ::_1));
    m_server.set_message_handler(bind(&Server::on_message,this,::_1,::_2));
    m_server.set_close_handler(bind(&Server::on_close,this,::_1));
    m_server.set_fail_handler(bind(&Server::on_fail,this, ::_1));
    
}

void Server::on_open(connection_hdl hdl) {
    //m_server.get_con_from_hdl(hdl)->set_pong_timeout(1000);
}

void Server::run(uint16_t port) {
    m_server.listen(port);

    m_server.start_accept();
    try {
        m_server.run();
    } catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    }
}

void Server::on_message(connection_hdl hdl, server::message_ptr msg) {
    if (msg->get_payload().length() == 0) return;


    MedLogger::instance()->warning(msg->get_payload());

    Message message;
    message << "m";

    std::string usr = "";
    std::vector<std::string> parts = split(msg->get_payload());

    if (parts.size() > 2) {
        if (!checkAuth(msg->get_payload(), hdl)) {
            m_server.close(hdl, websocketpp::close::status::invalid_payload, "mConnection refused! No authorization");
            return;
        }
        lock_guard<mutex> guard(m_action_lock);
        usr = getClient(hdl)->username;
        this->m_server.send(hdl, "mLoged in as user: " + usr, msg->get_opcode());
        return;
    }
    
    {
        lock_guard<mutex> guard(m_action_lock);
        Client* c = getClient(hdl);
        if (c == nullptr) {
            m_server.close(hdl, websocketpp::close::status::invalid_payload, "mConnection refused! No authorization");
            return;
        }
        usr = c->username;
    }

    
    try {        
        market->query(msg->get_payload(), message);

        this->m_server.send(hdl, message.str(), msg->get_opcode());
    } catch (websocketpp::exception const & e) {
        std::cout << "Failed because: "
                << "(" << e.what() << ")" << std::endl;
    }
}

void Server::on_close(connection_hdl hdl) {
    lock_guard<mutex> guard(m_action_lock);
    Client* client = getClient(hdl);
    if (client) {
        connections.erase(std::remove(connections.begin(), connections.end(),  client), connections.end());
        delete client;
    }
}

void Server::on_fail(connection_hdl hdl) {
    lock_guard<mutex> guard(m_action_lock);
    Client* client = getClient(hdl);
    if (client) {
        connections.erase(std::remove(connections.begin(), connections.end(),  client), connections.end());
        delete client;
    }
}

Server::~Server() {
    running = false;

    thr.join();

    delete vault;

    for (Client* c : connections) {
        delete c;
    }
} 

std::vector<std::string> Server::split(std::string s) {
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


bool Server::checkAuth(std::string header, connection_hdl hdl) {
    std::vector<std::string> headerVector = split(header);
    
    {
        lock_guard<mutex> guard(m_action_lock);
        Client* client = getClient(hdl);
        if (std::find(connections.begin(), connections.end(), client)  != connections.end()) {
            MedLogger::instance()->error("Already logged in!!");
            return true;
        }
    }

    if (headerVector[0] == "login" && headerVector.size() == 3) {
        return trySignin(headerVector[1], headerVector[2], hdl);
    }

    if (headerVector[0] == "register" && headerVector.size() == 3) {
        return trySignup(headerVector[1], headerVector[2], hdl);
    }

    if (headerVector[0] == "update" && headerVector.size() == 4) {
        return tryUpdate(headerVector[1], headerVector[2], headerVector[3], hdl);
    }

    MedLogger::instance()->error("Received uknown header!");
    return false;
}

bool Server::trySignin(std::string username, std::string password, connection_hdl hdl) {
    if (vault->signin(username, password)) {
        {
            lock_guard<mutex> guard(m_action_lock);
            connections.push_back(new Client(username, hdl));
        }
        return true;
    }

    MedLogger::instance()->error("Failed to authenticated!");
    return false;
}

bool Server::trySignup(std::string username, std::string password, connection_hdl hdl) {
    if (vault->signup(username, password)) {
        {
            lock_guard<mutex> guard(m_action_lock);
            connections.push_back(new Client(username, hdl));
        }
        return true;
    }

    MedLogger::instance()->error("Failed to sign in!");
    return false;
}

bool Server::tryUpdate(std::string username, std::string oldPassword, std::string newPassword, connection_hdl hdl) {
    if (vault->update(username, oldPassword, newPassword)) {
        {
            lock_guard<mutex> guard(m_action_lock);
            connections.push_back(new Client(username, hdl));
        }
        return true;
    }

    MedLogger::instance()->error("failed to update password!");
    return false;
}

Client* Server::getClient(connection_hdl hdl) {
    for (Client* cl : connections) {
        if (cl->hdl.lock() == hdl.lock()){
            return cl;
        }
    }

    return nullptr;
}

void Server::broadcast() {
    while (running) {
        {
            lock_guard<mutex> guard(m_action_lock);
            MedLogger::instance()->warning(std::to_string(connections.size()));
            if (connections.size() > 0) {
                Message m; 
                m << "b";
                market->select("Broadcast where price > 0", m);
                for (Client* client : connections) {
                    try {
                        m_server.send(client->hdl, m.str(), websocketpp::frame::opcode::text);
                    }
                    catch (...) { 
                        MedLogger::instance()->log("Error for client, disconnected!"); 
                    }
                }
            }
        }
        std::chrono::milliseconds dura(250);
        std::this_thread::sleep_for(dura);
    }
}

// m - regularni response
// b - broadcast response