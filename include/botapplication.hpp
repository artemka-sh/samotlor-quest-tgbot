#pragma once

#include <tgbot/tgbot.h>
#include "config.hpp"
#include "questions.hpp"
#include "database_manager.hpp"
#include "telegram_messenger.hpp"
#include <memory>


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
    bool isValidAnswer(const std::string& userText, const Question question);

    std::unique_ptr<Config>             config;
    std::unique_ptr<Questions>          questions;
    std::unique_ptr<TgBot::Bot>         bot;
    std::unique_ptr<DatabaseManager>    databaseManager;
    std::unique_ptr<TelegramMessenger>  messenger;
    std::unique_ptr<TgBot::TgLongPoll>  longPoll;  
};