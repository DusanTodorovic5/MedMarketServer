#include "../hpp/server.hpp"
#include "../hpp/med_logger.hpp"
#include "../hpp/med_file_logger.hpp"
#include <functional>

MedServer::MedServer() {
    MedLogger::init(new FileMedLogger());
    market = new MedicineMarket();

    server.Get("/login", [this](const httplib::Request& req, httplib::Response& res){
        std::string result = this->signin(req);

        res.set_content(result, "text/plain");
    });

    server.Get("/select", [this](const httplib::Request& req, httplib::Response& res){
        std::pair<bool, std::string> result = this->checkToken(req);

        if (!result.first) {
            res.set_content(result.second, "text/plain");
            return;
        }

        std::string username = result.second;

        Message msg;
        std::string query = username + " where price >= 0";

        if (req.params.size()) {            
            query = username + " where";

            bool dirty = false;
            if (req.has_param("name")) {
                std::string operation = "=";

                if (req.has_param("nameOper")) {
                    operation = req.get_param_value("nameOper");
                }

                query += " name " + operation + " " + req.get_param_value("name");
                dirty = true;
            }

            if (req.has_param("boxes")) {
                std::string operation = "=";

                if (req.has_param("boxesOper")) {
                    operation = req.get_param_value("boxesOper");
                }

                if (operation == "between") {
                    std::vector<std::string> tmp = this->split(req.get_param_value("boxes"));

                    if (tmp.size() != 2) {
                        res.set_content("Bad Query!", "text/plain");
                        return;
                    }
                    if (dirty) {
                        query += " and";
                    }
                    query += " boxes >= " + tmp[0] + " and boxes <= " + tmp[1];
                } else {
                    if (dirty) {
                        query += " and";
                    }
                    query += " boxes " + operation + " " + req.get_param_value("boxes");
                }
                dirty = true;
            }

            if (req.has_param("price")) {
                std::string operation = "=";

                if (req.has_param("priceOper")) {
                    operation = req.get_param_value("priceOper");
                }

                if (operation == "between") {
                    std::vector<std::string> tmp = this->split(req.get_param_value("price"));

                    if (tmp.size() != 2) {
                        res.set_content("Bad Query!", "text/plain");
                        return;
                    }
                    if (dirty) {
                        query += " and";
                    }
                    query += " price >= " + tmp[0] + " and price <= " + tmp[1];
                } else {
                    if (dirty) {
                        query += " and";
                    }
                    query += " price " + operation + " " + req.get_param_value("price");
                }
                dirty = true;
            }

            if (req.has_param("purpose")) {
                std::string operation = "=";

                if (req.has_param("purposeOper")) {
                    operation = req.get_param_value("purposeOper");
                }

                if (dirty) {
                    query += " and";
                }
                query += " purpose " + operation + " " + req.get_param_value("purpose");
                dirty = true;
            }

            if (req.has_param("effectiveness")) {
                std::string operation = "=";

                if (req.has_param("effectivenessOper")) {
                    operation = req.get_param_value("effectivenessOper");
                }

                if (operation == "between") {
                    std::vector<std::string> tmp = this->split(req.get_param_value("effectiveness"));

                    if (tmp.size() != 2) {
                        res.set_content("Bad Query!", "text/plain");
                        return;
                    }
                    if (dirty) {
                        query += " and";
                    }
                    query += " effectiveness >= " + tmp[0] + " and effectiveness <= " + tmp[1];
                } else {
                    if (dirty) {
                        query += " and";
                    }
                    query += " effectiveness " + operation + " " + req.get_param_value("effectiveness");
                }
                dirty = true;
            }

            std::cout << query << std::endl;
        }
        
        market->select(query, msg);

        res.set_content(msg.str(), "text/plain");
    });

    server.Post("/buy", [this](const httplib::Request& req, httplib::Response& res){
        std::pair<bool, std::string> result = this->checkToken(req);

        if (!result.first) {
            res.set_content(result.second, "text/plain");
            return;
        }

        std::string username = result.second;

        Message msg;
        std::string query = username + " where " + req.body;
        
        market->buy(query, msg);

        res.set_content(msg.str(), "text/plain");
    });

    server.Post("/buyAnyways", [this](const httplib::Request& req, httplib::Response& res){
        std::pair<bool, std::string> result = this->checkToken(req);

        if (!result.first) {
            res.set_content(result.second, "text/plain");
            return;
        }

        std::string username = result.second;

        if (!req.has_param("query")) {
            res.set_content("Error: Query not provided!", "text/plain");
        }
        
        Message msg;
        std::string query = username + " where " + req.body;
        
        market->buy(query, msg, true);

        res.set_content(msg.str(), "text/plain");
    });

    server.Post("/signup", [this](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_header("Username") || !req.has_header("Password")) {
            res.set_content("Authorization header not provided", "text/plain");
            return;
        }

        if (!this->signup(req.get_header_value("Username"), req.get_header_value("Password"))) {
            res.set_content("Unable to signup: Username already in use", "text/plain");
            return;
        }

        res.set_content("Succesfully registered!", "text/plain");
    });
}

MedServer::~MedServer() {
    delete market;
    MedLogger::finish();
}

void MedServer::listen() {
    server.listen("localhost", 8100);
}

bool MedServer::signup(std::string username, std::string password) {
    return this->vault.signup(username, password);
}

std::string MedServer::signin(const httplib::Request& req) {
    if (!req.has_header("Username") || !req.has_header("Password")) {
        return "Authorization header not provided";
    }
    std::string token = this->vault.signin(req.get_header_value("Username"), req.get_header_value("Password"));
    if (token == "") {
        return "Unable to login: Incorrect username or password";
    }

    return token;
}

std::pair<bool, std::string> MedServer::checkToken(const httplib::Request& req) {
    if (!req.has_header("Token")) {
        return {false, "Authorization header not provided"};
    }

    return this->vault.checkToken(req.get_header_value("Token"));
}

std::vector<std::string> MedServer::split(std::string s, std::string delimiter) {
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