#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>

// Структура для одного опроса
struct Survey {
    int id;
    std::string question;
    std::vector<std::string> answers;
    bool allowCustomAnswer;
    int calledQuestId;
};

class Surveys {
public:
    QJsonObject data; // Хранит весь JSON-объект опросов
    std::vector<Survey> surveys; // Вектор опросов для удобного доступа

    Surveys(const std::string path) {
        readSurveysFromFile(path);
        parseSurveys();
    }

    // Получение опроса по ID
    const Survey* findById(int id) const {
        for (const Survey& survey : surveys) {
            if (survey.id == id) {
                return &survey;
            }
        }
        return nullptr;
    }

    // Проверка, пуста ли коллекция
    bool isEmpty() const {
        return surveys.empty();
    }

private:
    void readSurveysFromFile(const std::string& pathx) {
        std::string path = "res/surveys.json";
        if (path.empty()) {
            throw std::runtime_error("Surveys file path is empty");
        }

        std::ifstream surveyFile(path);
        if (!surveyFile.is_open()) {
            throw std::runtime_error("Failed to open surveys file: " + path);
        }

        std::string content((std::istreambuf_iterator<char>(surveyFile)),
                            std::istreambuf_iterator<char>());
        QJsonDocument doc = QJsonDocument::fromJson(QByteArray(content.c_str(), static_cast<int>(content.size())));
        if (doc.isNull() || !doc.isObject()) {
            surveyFile.close();
            throw std::runtime_error("Failed to parse surveys JSON");
        }

        data = doc.object();
        surveyFile.close();
    }

    void parseSurveys() {
        QJsonArray surveyArray = data["surveys"].toArray();
        if (surveyArray.isEmpty()) {
            throw std::runtime_error("No surveys found in JSON");
        }

        surveys.clear();
        for (const QJsonValue& value : surveyArray) {
            if (!value.isObject()) {
                continue;
            }
            QJsonObject obj = value.toObject();

            Survey survey;
            survey.id = obj["id"].toInt();
            survey.question = obj["question"].toString().toStdString();

            QJsonArray answers = obj["answers"].toArray();
            for (const QJsonValue& ans : answers) {
                survey.answers.push_back(ans.toString().toStdString());
            }

            survey.allowCustomAnswer = obj["allowCustomAnswer"].toBool();
            survey.calledQuestId = obj["calledQuestId"].toInt();

            surveys.push_back(survey);
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
