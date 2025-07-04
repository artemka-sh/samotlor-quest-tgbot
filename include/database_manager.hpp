#pragma once

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QString>
#include "config.hpp"

class DatabaseManager {
public:
    DatabaseManager(const Config& config);
    ~DatabaseManager();

    bool addUser(qint64 telegramId, const QString& username, const QString& firstName, const QString& lastName);
    void initTables();
    bool hasUser(qint64 telegramId);
    int getLastAnsweredQuestionId(qint64 telegramId);
    void saveAnswer(qint64 telegramId, int questionId, const std::string& answer, const std::string& type);
    void updateAnswer(qint64 telegramId, int questionId, const std::string& answer, const std::string& type);
    int getUserId(qint64 telegramId);

private:
    QSqlDatabase db;
};