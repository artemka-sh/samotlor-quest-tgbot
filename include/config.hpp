#pragma once

#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

class Config
{
public:    
    json data;
    std::string telegram_token;
    Config()
    {
        readConfigFromFile("res/config.json");
        telegram_token = data["telegram_token"];
    }
    
private:
    void readConfigFromFile(std::string path)
    {
        if(path.empty()) throw std::runtime_error("config path empty");

        std::ifstream configfile(path);
        data = json::parse(configfile);
    }
};
