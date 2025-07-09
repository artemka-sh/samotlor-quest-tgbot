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



User BotApplication::getUser(TgBot::Message::Ptr message) {
    qint64 userId = message->from->id;
    std::string userName = message->from->username;
    QString firstName = message->from->firstName.empty() ? "" : QString::fromStdString(message->from->firstName);
    QString lastName = message->from->lastName.empty() ? "" : QString::fromStdString(message->from->lastName);
    if (!databaseManager->hasUser(userId)) {
        databaseManager->addUser(userId, QString::fromStdString(userName), firstName, lastName);
    }
    return User{userId, userName, firstName, lastName};
}


int BotApplication::getLastQuestionId() const {
    if (questions->questions.empty()) return -1;
    return questions->questions.back().id;
}

int BotApplication::getFirstQuestionId() const {
    if (questions->questions.empty()) return -1;
    return questions->questions.front().id;
}

const Question* BotApplication::getNextQuestion(int currentQuestionId) const {
    return questions->getNext(currentQuestionId);
}

int BotApplication::getLastAnsweredQuestionId(qint64 userId) const {
    return databaseManager->getLastAnsweredQuestionId(userId);
}

bool BotApplication::isValidAnswer(const std::string& userText, const Question question) {
    if (question.allowCustomAnswer) return true;
    if (question.correctAnswer.empty()) {
        for (const auto& ans : question.answers) {
            if (userText == ans) return true;
        }
    } else {
        return userText == question.correctAnswer;
    }
    return false;
}


void BotApplication::onAnyMessage(TgBot::Message::Ptr message) {
   

    // 1. Получение пользователя (создаём, если нет)
    User user = getUser(message);

    // 2. Получаем id последнего отвеченного вопроса
    int lastAnsweredId = getLastAnsweredQuestionId(user.id);
    if (lastAnsweredId == getLastQuestionId() && databaseManager->isQuestionAnswered(user.id, lastAnsweredId)) {
        messenger->sendMessage(user.id, "Вы завершили квест! Спасибо за участие.");
        return;
    }
    
 
    //3. Проверка условия на команду /start
    if (message->text == "/start" || lastAnsweredId < 0) {
        // 4. Сохраняем первый вопрос без текста ответа
        if (lastAnsweredId >= 0) {
            messenger->sendQuestionWithKeyboard(user.id, *questions->findById(lastAnsweredId));
        } else {
            // ...existing code...
            messenger->sendMessage(user.id,
                "Друзья, у нас с вами уникальный шанс — создать самую честную картину нашей команды и представить ее на празднике в честь юбилея собственника!\n\n"
                "Почему это важно?\n"
                "🔹 Ваше мнение станет частью истории компании — мы соберем все ответы и покажем, какой реальный «климат» в команде 😉\n"
                "🔹 Мы увидим реальные суперсилы нас, как команды\n"
                "🔹 Собственник лично прокомментирует ключевые выводы\n\n"
                "Как это работает?\n"
                "1️⃣ Анонимно отвечаете на вопросы (15-20 минут)\n"
                "2️⃣ Бот анализирует все ответы\n"
                "3️⃣ Ждем развязки на празднике — мы превратим сухие цифры в мощный сторителлинг\n\n"
                "💡 Это не просто опрос — это наш с вами коллективный портрет!\n\n"
                "Давайте сделаем это честно, круто и с пользой для всех! И… у нас есть всего 2 дня!"
            );
            // ...existing code...
            const Question* firstQuestion = questions->findById(getFirstQuestionId());
            databaseManager->saveAnswer(user.id, firstQuestion->id, "", firstQuestion->type);
            messenger->sendQuestionWithKeyboard(user.id, *firstQuestion);
        }
        return;
    }
    const Question* lastQuestion = questions->findById(lastAnsweredId);


    // 5. Обновляем предыдущий вопрос (сохраняем ответ)
    if (isValidAnswer(message->text, *lastQuestion)) {
        databaseManager->updateAnswer(user.id, lastAnsweredId, message->text, lastQuestion->type);
    }
    else{
        messenger->sendMessage(user.id, "Ответ не верный, попробуйте ещё раз.");
        messenger->sendQuestionWithKeyboard(user.id, *questions->findById(lastAnsweredId));
        return;
    }
    
    // 6. Получаем следующий вопрос
    const Question* nextQuestion = nullptr;
    if (lastAnsweredId <= 0) {
        nextQuestion = questions->findById(1);
    } else {
        nextQuestion = getNextQuestion(lastAnsweredId);
    }
    if (!nextQuestion) {
        messenger->sendMessage(user.id, "Вы завершили квест! Спасибо за участие.");
        return;
    }
    // 7. Сохраняем следующий вопрос без текста ответа
    databaseManager->saveAnswer(user.id, nextQuestion->id, "", nextQuestion->type);
    // 8. Отправляем следующий вопрос
    messenger->sendQuestionWithKeyboard(user.id, *nextQuestion);
}
