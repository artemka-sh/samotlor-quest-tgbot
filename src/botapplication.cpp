#include "botapplication.hpp"
#include <iostream>
#include <algorithm>
#include "telegram_messenger.hpp"
#include <QDebug>

BotApplication::BotApplication() {
    config = new Config("res/config.json");
    questions = new Questions("res/questions.json");
    bot = new TgBot::Bot(config->telegram_token);
    databaseManager = new DatabaseManager(*config);
    messenger = new TelegramMessenger(bot);
    
    databaseManager->initTables();
    databaseManager->addUser(1234567890, "test", "test", "test");

    bot->getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
        bot->getApi().sendMessage(message->chat->id, "Привет! Пройди анонимный опрос и т.д. и т.п.");
    });

    bot->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
        this->onAnyMessage(message);
    });
}

BotApplication::~BotApplication() {
    delete bot;
    delete questions;
    delete config;
    delete messenger;
}

void BotApplication::startBot() {
    try {
        std::cout << "Bot username: " << bot->getApi().getMe()->username << std::endl;
        longPoll = new TgBot::TgLongPoll(*bot);
        while (true) {
            std::cout << "Long poll started" << std::endl;
            longPoll->start();
        }
    } catch (const TgBot::TgException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void BotApplication::runLongPoll() {
    // Не используется, оставлено для совместимости
}

void BotApplication::onAnyMessage(TgBot::Message::Ptr message) {
    qint64 userId = message->from->id;
    std::string userName = message->from->username;
    std::string userText = message->text;
    QString firstName = message->from->firstName.empty() ? "" : QString::fromStdString(message->from->firstName);
    QString lastName = message->from->lastName.empty() ? "" : QString::fromStdString(message->from->lastName);

    qDebug() << "[onAnyMessage] userId:" << userId << "userName:" << QString::fromStdString(userName)
             << "userText:" << QString::fromStdString(userText);

    // 1. Проверка пользователя
    ensureUserInDatabase(userId, userName, firstName, lastName);

    // 2. Получение id последнего отвеченного вопроса
    int lastAnsweredId = getLastAnsweredQuestionId(userId);
    qDebug() << "[onAnyMessage] lastAnsweredId:" << lastAnsweredId;

    // 3. Если пользователь не отвечал ни на один вопрос — просто задать первый вопрос
    if (handleFirstQuestionIfNeeded(userId, lastAnsweredId)) {
        return;
    }

    // 4. Получение предыдущего вопроса
    const Question* prevQuestion = getPreviousQuestion(lastAnsweredId);
    if (!prevQuestion) {
        messenger->sendMessage(userId, "Ошибка: вопрос по айди не найден.");
        return;
    }
    qDebug() << "[onAnyMessage] prevQuestion id:" << prevQuestion->id;

    // 5. Проверка валидности ответа
    if (handleInvalidAnswer(userId, userText, prevQuestion)) {
        return;
    }
    qDebug() << "[onAnyMessage] Ответ валиден, сохраняем...";

    // 6. Сохранение ответа
    saveUserAnswer(userId, prevQuestion, userText);

    // 7. Получение следующего вопроса
    const Question* nextQuestion = getNextQuestion(prevQuestion->id);
    qDebug() << "[onAnyMessage] nextQuestion id:" << (nextQuestion ? nextQuestion->id : -1);

    // 8. Если вопросов больше нет
    if (handleNoMoreQuestions(userId, nextQuestion)) {
        return;
    }

    // 9. Фидбек по предыдущему вопросу
    sendFeedbackForPrevious(prevQuestion, userText, userId);

    // 10. Отправка следующего вопроса
    messenger->sendQuestionWithKeyboard(userId, *nextQuestion);
}

bool BotApplication::isValidAnswer(const std::string& userText, const Question& question) {
    if (question.allowCustomAnswer) return true;
    for (const auto& ans : question.answers) {
        if (userText == ans) return true;
    }
    if (!question.correctAnswer.empty() && userText == question.correctAnswer) return true;
    return false;
}

void BotApplication::ensureUserInDatabase(qint64 userId, const std::string& userName, const QString& firstName, const QString& lastName) {
    if (!databaseManager->hasUser(userId)) {
        qDebug() << "[ensureUserInDatabase] User not found, adding...";
        databaseManager->addUser(userId, QString::fromStdString(userName), firstName, lastName);
    }
}

int BotApplication::getLastAnsweredQuestionId(qint64 userId) {
    return databaseManager->getLastAnsweredQuestionId(userId);
}

bool BotApplication::handleFirstQuestionIfNeeded(qint64 userId, int lastAnsweredId) {
    if (lastAnsweredId == 0 || lastAnsweredId == -1) {
        const Question* firstQuestion = questions->findById(1);
        if (firstQuestion) {
            messenger->sendQuestionWithKeyboard(userId, *firstQuestion);
        } else {
            messenger->sendMessage(userId, "Ошибка: первый вопрос не найден.");
        }
        return true;
    }
    return false;
}

const Question* BotApplication::getPreviousQuestion(int lastAnsweredId) {
    return questions->findById(lastAnsweredId);
}

bool BotApplication::handleInvalidAnswer(qint64 userId, const std::string& userText, const Question* prevQuestion) {
    if (!isValidAnswer(userText, *prevQuestion)) {
        qDebug() << "[handleInvalidAnswer] Ответ невалиден!";
        messenger->sendMessage(userId, "Пожалуйста, выберите один из предложенных вариантов ответа.");
        messenger->sendQuestionWithKeyboard(userId, *prevQuestion);
        return true;
    }
    return false;
}

void BotApplication::saveUserAnswer(qint64 userId, const Question* prevQuestion, const std::string& userText) {
    databaseManager->saveAnswer(userId, prevQuestion->id, userText, prevQuestion->type);
}

const Question* BotApplication::getNextQuestion(int lastAnsweredId) {
    return questions->getNext(lastAnsweredId);
}

bool BotApplication::handleNoMoreQuestions(qint64 userId, const Question* nextQuestion) {
    if (!nextQuestion) {
        messenger->sendMessage(userId, "Вы прошли все вопросы! Спасибо за участие.");
        return true;
    }
    return false;
}

void BotApplication::sendFeedbackForPrevious(const Question* prevQuestion, const std::string& userText, qint64 userId) {
    if (prevQuestion->type == "quest" && !prevQuestion->correctAnswer.empty()) {
        if (userText == prevQuestion->correctAnswer) {
            messenger->sendMessage(userId, "Верно! " + prevQuestion->explanation);
        } else {
            messenger->sendMessage(userId, "Неверно. " + prevQuestion->explanation);
        }
    } else if (!prevQuestion->explanation.empty()) {
        messenger->sendMessage(userId, prevQuestion->explanation);
    }
}