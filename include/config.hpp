#pragma once

#include <boost/json.hpp>
#include <fstream>

using json = boost::json::value;

class Config
{
public:    
    json data;
    std::string telegram_token;
    std::string db_addres;
    std::string db_port;
    std::string db_user;
    std::string db_password;

    Config()
    {
        readConfigFromFile("res/config.json");
        telegram_token = data.as_object()["telegram_token"].as_string().c_str();
        db_addres = data.as_object()["db_addres"].as_string().c_str();
        db_port = data.as_object()["db_port"].as_string().c_str();
        db_user = data.as_object()["db_user"].as_string().c_str();
        db_password = data.as_object()["db_password"].as_string().c_str();
    }
    
private:
    void readConfigFromFile(std::string path)
    {
        if (path.empty()) throw std::runtime_error("config path empty");

        std::ifstream configfile(path);
        std::string content((std::istreambuf_iterator<char>(configfile)),
                            std::istreambuf_iterator<char>());
        data = boost::json::parse(content);
    }
};