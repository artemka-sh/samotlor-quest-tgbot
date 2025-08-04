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
    std::string greeting; // вступительная часть квеста
    std::string ending; //завершающая часть квеста
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

    const Question* getNext(int id) const {
        bool found = false;
        for (const auto& q : questions) {
            if (q.id == id) {
                found = true;
                continue;
            }
            if (found) return &q;
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
        greeting = root["greeting"].toString().toStdString();
        ending = root["ending"].toString().toStdString();

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



