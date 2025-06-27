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

    Surveys() {
        readSurveysFromFile("res/surveys.json");
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
    void readSurveysFromFile(const std::string& path) {
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