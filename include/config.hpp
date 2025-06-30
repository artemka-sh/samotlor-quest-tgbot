#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <fstream>
#include <stdexcept>

class Config
{
public:    
    QJsonObject data;
    std::string telegram_token;
    std::string db_addres;
    std::string db_port;
    std::string db_user;
    std::string db_password;

    Config(std::string path)
    {
        readConfigFromFile(path);
        telegram_token = data["telegram_token"].toString().toStdString();
        db_addres = data["db_address"].toString().toStdString();
        db_port = data["db_port"].toString().toStdString();
        db_user = data["db_user"].toString().toStdString();
        db_password = data["db_password"].toString().toStdString();
    }
    
private:
    void readConfigFromFile(std::string path)
    {
        if(path.empty()) throw std::runtime_error("config path empty");

        std::ifstream configFile(path);
        if (!configFile.is_open()) {
            throw std::runtime_error("failed to open config file");
        }

        std::string content((std::istreambuf_iterator<char>(configFile)),
                             std::istreambuf_iterator<char>());
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray(content.c_str(), static_cast<int>(content.size())));
        if (doc.isNull()) {
            throw std::runtime_error("failed to parse config JSON");
        }

        data = doc.object();
    }
};