#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>

// Структура для одного квеста
struct Quest {
    int id;
    std::string question;
    std::vector<std::string> answers;
    int correctAnswer;
    std::string explanation;
    std::string difficulty;
};

class Quests {
public:
    QJsonObject data; // Хранит весь JSON-объект квестов
    std::vector<Quest> quests; // Вектор квестов для удобного доступа

    Quests(const std::string& path) {
        readQuestsFromFile(path);
        parseQuests();
    }

    // Получение квеста по ID
    const Quest* findById(int id) const {
        for (const Quest& quest : quests) {
            if (quest.id == id) {
                return &quest;
            }
        }
        return nullptr;
    }

    // Проверка, пуста ли коллекция
    bool isEmpty() const {
        return quests.empty();
    }

private:
    void readQuestsFromFile(const std::string& path) {
        if (path.empty()) {
            throw std::runtime_error("Quests file path is empty");
        }

        std::ifstream questFile(path);
        if (!questFile.is_open()) {
            throw std::runtime_error("Failed to open quests file: " + path);
        }

        std::string content((std::istreambuf_iterator<char>(questFile)),
                            std::istreambuf_iterator<char>());
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray(content.c_str(), static_cast<int>(content.size())));
        if (doc.isNull() || !doc.isObject()) {
            questFile.close();
            throw std::runtime_error("Failed to parse quests JSON");
        }

        data = doc.object();
        questFile.close();
    }

    void parseQuests() {
        QJsonArray questArray = data["questions"].toArray();
        if (questArray.isEmpty()) {
            throw std::runtime_error("No quests found in JSON");
        }

        quests.clear();
        for (const QJsonValue& value : questArray) {
            if (!value.isObject()) {
                continue;
            }
            QJsonObject obj = value.toObject();

            Quest quest;
            quest.id = obj["id"].toInt();
            quest.question = obj["question"].toString().toStdString();

            QJsonArray answers = obj["answers"].toArray();
            for (const QJsonValue& ans : answers) {
                quest.answers.push_back(ans.toString().toStdString());
            }

            quest.correctAnswer = obj["correctAnswer"].toInt();
            quest.explanation = obj["explanation"].toString().toStdString();
            quest.difficulty = obj["difficulty"].toString().toStdString();

            quests.push_back(quest);
        }
    }
}; 