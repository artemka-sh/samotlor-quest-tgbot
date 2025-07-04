#include "telegram_messenger.hpp"

TelegramMessenger::TelegramMessenger(TgBot::Bot* bot) : bot(bot) {}

void TelegramMessenger::sendMessage(qint64 userId, const std::string& text) {
    bot->getApi().sendMessage(userId, text);
}

void TelegramMessenger::sendQuestionWithKeyboard(qint64 userId, const Question question) {
    std::string text = question.question;
    TgBot::ReplyKeyboardMarkup::Ptr keyboard(new TgBot::ReplyKeyboardMarkup);
    keyboard->resizeKeyboard = true;
    keyboard->oneTimeKeyboard = true;
    if (!question.answers.empty()) {
        keyboard->keyboard.clear();
        for (const auto& ans : question.answers) {
            std::vector<TgBot::KeyboardButton::Ptr> row;
            TgBot::KeyboardButton::Ptr btn(new TgBot::KeyboardButton);
            btn->text = ans;
            row.push_back(btn);
            keyboard->keyboard.push_back(row);
        }
    } else {
        keyboard = nullptr;
    }
    bot->getApi().sendMessage(userId, text, nullptr, nullptr, keyboard);
} 