#pragma once
#include <tgbot/tgbot.h>
#include "questions.hpp"

class TelegramMessenger {
public:
    TelegramMessenger(TgBot::Bot* bot);
    void sendMessage(qint64 userId, const std::string& text);
    void sendQuestionWithKeyboard(qint64 userId, const Question& question);
private:
    TgBot::Bot* bot;
}; 