#pragma once

#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

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
        telegram_token = data["telegram_token"];
        db_addres = data["telegram_token"];
        db_port = data["db_port"];
        db_user = data["telegram_token"];
        db_password = data["telegram_token"];
    }
    
private:
    void readConfigFromFile(std::string path)
    {
        if(path.empty()) throw std::runtime_error("config path empty");

        std::ifstream configfile(path);
        data = json::parse(configfile);
    }
};
