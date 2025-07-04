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

    // Новые вспомогательные этапы:
    void ensureUserInDatabase(qint64 userId, const std::string& userName, const QString& firstName, const QString& lastName);
    int getLastAnsweredQuestionId(qint64 userId);
    bool handleFirstQuestionIfNeeded(qint64 userId, int lastAnsweredId);
    const Question* getPreviousQuestion(int lastAnsweredId);
    bool handleInvalidAnswer(qint64 userId, const std::string& userText, const Question* prevQuestion);
    void saveUserAnswer(qint64 userId, const Question* prevQuestion, const std::string& userText);
    const Question* getNextQuestion(int lastAnsweredId);
    bool handleNoMoreQuestions(qint64 userId, const Question* nextQuestion);
    void sendFeedbackForPrevious(const Question* prevQuestion, const std::string& userText, qint64 userId);
    
    TelegramMessenger* messenger;
    Config* config;
    Questions* questions;
    DatabaseManager* databaseManager;
    TgBot::Bot* bot;
    TgBot::TgLongPoll* longPoll;
};