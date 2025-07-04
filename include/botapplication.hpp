#pragma once

#include <tgbot/tgbot.h>
#include "config.hpp"
#include "questions.hpp"
#include "database_manager.hpp"
#include "telegram_messenger.hpp"

struct User{
    qint64 id;
    std::string username;
    QString firstName;
    QString lastName;
};

class BotApplication {
public:
    BotApplication();
    ~BotApplication();

    void startBot();
    
private:
    void onAnyMessage(TgBot::Message::Ptr message);
    User getUser(TgBot::Message::Ptr message);
    void updateQuestion(qint64 userId, int questionId, const std::string& answerText);
    int getLastQuestionId() const;
    int getFirstQuestionId() const;
    const Question* getNextQuestion(int currentQuestionId) const;
    int getLastAnsweredQuestionId(qint64 userId) const;
    bool isValidAnswer(const std::string& userText, const Question& question);

    TelegramMessenger* messenger;
    Config* config;
    Questions* questions;
    DatabaseManager* databaseManager;
    TgBot::Bot* bot;
    TgBot::TgLongPoll* longPoll;
};