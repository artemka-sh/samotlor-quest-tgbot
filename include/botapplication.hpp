#pragma once

#include <tgbot/tgbot.h>
#include "config.hpp"
#include "surveys.hpp"

class BotApplication {
public:
    BotApplication();
    ~BotApplication();
    void startBot();
    
private:
    
    void runLongPoll();
    Config* config;
    Surveys* surveys;
    TgBot::Bot* bot;
    TgBot::TgLongPoll* longPoll;
};