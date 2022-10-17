
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/client.hpp>
#include <mutex>
#include <iostream>
#include <string>

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using websocketpp::lib::thread;
using websocketpp::lib::mutex;
using websocketpp::lib::lock_guard;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

std::string open_msg = "";

std::string response = "";
std::mutex responseMutex;

std::string broadcastResponse = "";
std::mutex broadcastMutex;

int row,col;
client c;
websocketpp::connection_hdl g_hdl;

void on_open(client* c, websocketpp::connection_hdl hdl) {
    c->send(hdl, open_msg, websocketpp::frame::opcode::text);
}

void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
    websocketpp::lib::error_code ec;
    if (msg->get_payload().length() < 2) {
        return;
    }

    if (msg->get_payload()[0] == 'b') {
        lock_guard<mutex> guard(broadcastMutex);
        broadcastResponse = msg->get_payload().substr(1, msg->get_payload().size() - 1);
        return;
    }

    lock_guard<mutex> guard(responseMutex);
    response = msg->get_payload().substr(1, msg->get_payload().size() - 1);
}

std::string getLineFromCin() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void draw() {
    auto future = std::async(std::launch::async, getLineFromCin);
    
    while (1) {    
        if (future.wait_for(std::chrono::milliseconds(50)) == std::future_status::ready) {
            auto line = future.get();
            future = std::async(std::launch::async, getLineFromCin);

            if (line == "end") {
                exit(0);
            }

            c.send(g_hdl, line, websocketpp::frame::opcode::text);
        }


        std::string m_response;
        std::string m_broadcast;
        
        {
            lock_guard<mutex> guard(broadcastMutex);
            m_broadcast = broadcastResponse;
        }

        {
            lock_guard<mutex> guard(responseMutex);
            m_response = response;
        }

        system("clear");

        std::cout << std::endl << m_response << std::endl << std::endl;
        std::cout << m_broadcast << std::endl;
    }
}

int main(int argc, char** argv) {
    std::string uri = "ws://localhost:9002";

    thread thr(draw);

    open_msg = argv[1];

    try {
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);

        c.init_asio();

        c.set_open_handler(bind(&on_open,&c,::_1));
        c.set_message_handler(bind(&on_message,&c,::_1,::_2));

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return 0;
        }

        c.connect(con);


        g_hdl = con->get_handle();
        c.run();

        
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
