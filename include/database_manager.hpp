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

    bool addUser(qint64 telegramId, const QString& username, const QString& message);
    void initTables();

private:
    QSqlDatabase db;
};