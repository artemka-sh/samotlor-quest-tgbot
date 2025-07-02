#pragma once

#include <tgbot/tgbot.h>
#include "config.hpp"
#include "questions.hpp"
#include "database_manager.hpp"

class BotApplication {
public:
    BotApplication();
    ~BotApplication();
    void startBot();
    
private:
    
    void runLongPoll();
    Config* config;
    Questions* questions;
    DatabaseManager* databaseManager;
    TgBot::Bot* bot;
    TgBot::TgLongPoll* longPoll;
};