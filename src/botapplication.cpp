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
    qDebug() << "[onAnyMessage] userId:" << userId << "userName:" << QString::fromStdString(userName) << "userText:" << QString::fromStdString(userText);

    QString firstName = message->from->firstName.empty() ? "" : QString::fromStdString(message->from->firstName);
    QString lastName = message->from->lastName.empty() ? "" : QString::fromStdString(message->from->lastName);
    if (!databaseManager->hasUser(userId)) {
        qDebug() << "[onAnyMessage] User not found, adding...";
        databaseManager->addUser(userId, QString::fromStdString(userName), firstName, lastName);
    }

    // 2. Получить id последнего отвеченного вопроса
    int lastAnsweredId = databaseManager->getLastAnsweredQuestionId(userId);
    if (lastAnsweredId == 0) { // если не найдено, пусть будет -1
        lastAnsweredId = -1;
    }
    qDebug() << "[onAnyMessage] lastAnsweredId:" << lastAnsweredId;

    // 3. Определить следующий вопрос для пользователя
    const Question* nextQuestion = nullptr;
    if (lastAnsweredId == -1) {
        // Первый вопрос
        if (!questions->questions.empty()) {
            nextQuestion = &questions->questions[0];
        }
    } else {
        // Найти текущий вопрос по id
        auto it = std::find_if(
            questions->questions.begin(),
            questions->questions.end(),
            [lastAnsweredId](const Question& q) { return q.id == lastAnsweredId; }
        );
        if (it != questions->questions.end() && ++it != questions->questions.end()) {
            nextQuestion = &(*it);
        }
    }
    qDebug() << "[onAnyMessage] nextQuestion id:" << (nextQuestion ? nextQuestion->id : -1);

    // Если вопросов больше нет
    if (!nextQuestion) {
        messenger->sendMessage(userId, "Вы прошли все вопросы! Спасибо за участие.");
        return;
    }

    // 4. Если это первый вопрос, просто отправить его
    if (lastAnsweredId == -1) {
        messenger->sendQuestionWithKeyboard(userId, *nextQuestion);
        return;
    }

    // 5. Проверить, валиден ли ответ пользователя на предыдущий вопрос
    const Question* prevQuestion = questions->findById(lastAnsweredId);
    if (!prevQuestion) {
        messenger->sendMessage(userId, "Ошибка: предыдущий вопрос не найден.");
        return;
    }
    qDebug() << "[onAnyMessage] prevQuestion id:" << prevQuestion->id;

    if (!isValidAnswer(userText, *prevQuestion)) {
        messenger->sendMessage(userId, "Пожалуйста, выберите один из предложенных вариантов ответа.");
        messenger->sendQuestionWithKeyboard(userId, *prevQuestion);
        return;
    }

    // 6. Сохранить ответ пользователя
    qDebug() << "[saveAnswer] called for telegramId:" << userId << "questionId:" << lastAnsweredId << "type:" << QString::fromStdString(prevQuestion->type);
    databaseManager->saveAnswer(userId, lastAnsweredId, userText, prevQuestion->type);

    // 7. Если предыдущий вопрос был квестовым, проверить правильность ответа
    if (prevQuestion->type == "quest" && !prevQuestion->correctAnswer.empty()) {
        if (isCorrectAnswer(userText, *prevQuestion)) {
            messenger->sendMessage(userId, "Верно! " + prevQuestion->explanation);
        } else {
            messenger->sendMessage(userId, "Неверно. " + prevQuestion->explanation);
        }
    } else if (!prevQuestion->explanation.empty()) {
        messenger->sendMessage(userId, prevQuestion->explanation);
    }

    // 8. Отправить следующий вопрос
    messenger->sendQuestionWithKeyboard(userId, *nextQuestion);
}

void BotApplication::sendQuestion(qint64 userId, const Question& question) {
    messenger->sendQuestionWithKeyboard(userId, question);
}

bool BotApplication::isValidAnswer(const std::string& userText, const Question& question) {
    if (question.allowCustomAnswer) return true;
    for (const auto& ans : question.answers) {
        if (userText == ans) return true;
    }
    if (!question.correctAnswer.empty() && userText == question.correctAnswer) return true;
    return false;
}

bool BotApplication::isCorrectAnswer(const std::string& userText, const Question& question) {
    return userText == question.correctAnswer;
}