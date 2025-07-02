#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>

struct Question {
    int id;
    std::string type;
    std::string question;
    std::vector<std::string> answers;
    bool allowCustomAnswer;
    std::string correctAnswer; // может быть пустой строкой, если нет
    std::string explanation;   // может быть пустой строкой, если нет
};

class Questions {
public:
    std::vector<Question> questions;

    Questions(const std::string& path) {
        QJsonObject root = loadJsonFromFile(path);
        parseQuestions(root);
    }

    const Question* findById(int id) const {
        for (const auto& q : questions) {
            if (q.id == id) return &q;
        }
        return nullptr;
    }

    bool isEmpty() const {
        return questions.empty();
    }

private:
    QJsonObject loadJsonFromFile(const std::string& path) {
        if (path.empty()) {
            throw std::runtime_error("Questions file path is empty");
        }
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open questions file: " + path);
        }
        std::string content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray(content.c_str(), static_cast<int>(content.size())));
        if (doc.isNull() || !doc.isObject()) {
            throw std::runtime_error("Failed to parse questions JSON");
        }
        return doc.object();
    }

    void parseQuestions(const QJsonObject& root) {
        if (!root.contains("questions") || !root["questions"].isArray()) {
            throw std::runtime_error("No questions found in JSON");
        }
        QJsonArray arr = root["questions"].toArray();
        questions.clear();
        for (const auto& item : arr) {
            if (!item.isObject()) continue;
            QJsonObject obj = item.toObject();
            Question q;
            q.id = obj["id"].toInt();
            q.type = obj["type"].toString().toStdString();
            q.question = obj["question"].toString().toStdString();
            q.allowCustomAnswer = obj.value("allowCustomAnswer").toBool(false);
            q.correctAnswer = obj.contains("correctAnswer") && !obj["correctAnswer"].isNull() ? obj["correctAnswer"].toString().toStdString() : "";
            q.explanation = obj.contains("explanation") && !obj["explanation"].isNull() ? obj["explanation"].toString().toStdString() : "";
            q.answers.clear();
            if (obj.contains("answers") && obj["answers"].isArray()) {
                for (const auto& ans : obj["answers"].toArray()) {
                    if (ans.isString()) {
                        q.answers.push_back(ans.toString().toStdString());
                    }
                }
            }
            questions.push_back(std::move(q));
        }
    }
};





//Альтернатинвый вариант

// #pragma once

// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QJsonArray>
// #include <fstream>
// #include <stdexcept>
// #include <vector>
// #include <string>

// struct Survey {
//     int id;
//     std::string question;
//     std::vector<std::string> answers;
//     bool allowCustomAnswer = false;
//     int calledQuestId = 0;
// };

// class Surveys {
// public:
//     std::vector<Survey> surveys;

//     explicit Surveys(const std::string& path) {
//         const QJsonObject root = loadJsonFromFile(path);
//         parseSurveys(root);
//     }

//     const Survey* findById(int id) const {
//         for (const auto& s : surveys) {
//             if (s.id == id) return &s;
//         }
//         return nullptr;
//     }

//     bool isEmpty() const {
//         return surveys.empty();
//     }

// private:
//     QJsonObject loadJsonFromFile(const std::string& path) {
//         if (path.empty()) {
//             throw std::runtime_error("Surveys file path is empty");
//         }

//         std::ifstream file(path);
//         if (!file.is_open()) {
//             throw std::runtime_error("Failed to open surveys file: " + path);
//         }

//         std::string content((std::istreambuf_iterator<char>(file)),
//                              std::istreambuf_iterator<char>());

//         QJsonParseError error;
//         QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(content), &error);
//         if (doc.isNull() || !doc.isObject()) {
//             throw std::runtime_error("Failed to parse JSON: " + error.errorString().toStdString());
//         }

//         return doc.object();
//     }

//     void parseSurveys(const QJsonObject& root) {
//         if (!root.contains("surveys") || !root["surveys"].isArray()) {
//             return;
//         }

//         QJsonArray surveyArray = root["surveys"].toArray();
//         surveys.clear();

//         for (const auto& item : surveyArray) {
//             if (!item.isObject()) continue;
//             const QJsonObject obj = item.toObject();

//             Survey s;

//             if (!obj.contains("id") || !obj["id"].isDouble()) continue;
//             if (!obj.contains("question") || !obj["question"].isString()) continue;

//             s.id = obj["id"].toInt();
//             s.question = obj["question"].toString().toStdString();

//             if (obj.contains("answers") && obj["answers"].isArray()) {
//                 for (const auto& ans : obj["answers"].toArray()) {
//                     if (ans.isString()) {
//                         s.answers.push_back(ans.toString().toStdString());
//                     }
//                 }
//             }

//             s.allowCustomAnswer = obj.value("allowCustomAnswer").toBool(false);
//             s.calledQuestId = obj.value("calledQuestId").toInt(0);

//             surveys.push_back(std::move(s));
//         }
//     }
// };








// то что было раньше в quests если нужно

// #pragma once

// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QJsonArray>
// #include <fstream>
// #include <stdexcept>
// #include <vector>
// #include <string>

// // Структура для одного квеста
// struct Quest {
//     int id;
//     std::string question;
//     std::vector<std::string> answers;
//     int correctAnswer;
//     std::string explanation;
//     std::string difficulty;
// };

// class Quests {
// public:
//     QJsonObject data; // Хранит весь JSON-объект квестов
//     std::vector<Quest> quests; // Вектор квестов для удобного доступа

//     Quests(const std::string& path) {
//         readQuestsFromFile(path);
//         parseQuests();
//     }

//     // Получение квеста по ID
//     const Quest* findById(int id) const {
//         for (const Quest& quest : quests) {
//             if (quest.id == id) {
//                 return &quest;
//             }
//         }
//         return nullptr;
//     }

//     // Проверка, пуста ли коллекция
//     bool isEmpty() const {
//         return quests.empty();
//     }

// private:
//     void readQuestsFromFile(const std::string& path) {
//         if (path.empty()) {
//             throw std::runtime_error("Quests file path is empty");
//         }

//         std::ifstream questFile(path);
//         if (!questFile.is_open()) {
//             throw std::runtime_error("Failed to open quests file: " + path);
//         }

//         std::string content((std::istreambuf_iterator<char>(questFile)),
//                             std::istreambuf_iterator<char>());
//         QJsonDocument doc = QJsonDocument::fromJson(QByteArray(content.c_str(), static_cast<int>(content.size())));
//         if (doc.isNull() || !doc.isObject()) {
//             questFile.close();
//             throw std::runtime_error("Failed to parse quests JSON");
//         }

//         data = doc.object();
//         questFile.close();
//     }

//     void parseQuests() {
//         QJsonArray questArray = data["questions"].toArray();
//         if (questArray.isEmpty()) {
//             throw std::runtime_error("No quests found in JSON");
//         }

//         quests.clear();
//         for (const QJsonValue& value : questArray) {
//             if (!value.isObject()) {
//                 continue;
//             }
//             QJsonObject obj = value.toObject();

//             Quest quest;
//             quest.id = obj["id"].toInt();
//             quest.question = obj["question"].toString().toStdString();

//             QJsonArray answers = obj["answers"].toArray();
//             for (const QJsonValue& ans : answers) {
//                 quest.answers.push_back(ans.toString().toStdString());
//             }

//             quest.correctAnswer = obj["correctAnswer"].toInt();
//             quest.explanation = obj["explanation"].toString().toStdString();
//             quest.difficulty = obj["difficulty"].toString().toStdString();

//             quests.push_back(quest);
//         }
//     }
// }; 

