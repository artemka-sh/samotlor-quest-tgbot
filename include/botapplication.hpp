#pragma once

#include <tgbot/tgbot.h>
#include "config.hpp"
#include "questions.hpp"
#include "database_manager.hpp"
#include "telegram_messenger.hpp"

class BotApplication {
public:
    BotApplication();
    ~BotApplication();

    void startBot();
    
private:
    void onAnyMessage(TgBot::Message::Ptr message);
    void sendQuestion(qint64 userId, const Question& question);
    bool isValidAnswer(const std::string& userText, const Question& question);
    bool isCorrectAnswer(const std::string& userText, const Question& question);
    void runLongPoll();
    
    TelegramMessenger* messenger;
    Config* config;
    Questions* questions;
    DatabaseManager* databaseManager;
    TgBot::Bot* bot;
    TgBot::TgLongPoll* longPoll;
};