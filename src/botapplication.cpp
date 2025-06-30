#include "botapplication.hpp"
#include <iostream>

BotApplication::BotApplication() {
    config = new Config("res/config.json");
    surveys = new Surveys("res/quests.json");
    bot = new TgBot::Bot(config->telegram_token);

    bot->getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {
        bot->getApi().sendMessage(message->chat->id, "Привет! Пройди анонимный опрос и т.д. и т.п.");
    });

    bot->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
        std::cout << "User wrote: " << message->text << std::endl;
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        bot->getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });
}

BotApplication::~BotApplication() {
    delete bot;
    delete surveys;
    delete config;
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