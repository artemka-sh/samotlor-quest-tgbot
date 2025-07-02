#include "database_manager.hpp"
#include <QDebug>

DatabaseManager::DatabaseManager(const Config& config) {
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(QString::fromStdString(config.db_addres));
    db.setPort(QString::fromStdString(config.db_port).toInt());
    db.setDatabaseName(QString::fromStdString(config.db_name)); 
    db.setUserName(QString::fromStdString(config.db_user));
    db.setPassword(QString::fromStdString(config.db_password));
    if (!db.open()) {
        qDebug() << "Failed to connect to database:" << db.lastError().text();
    } else {
        qDebug() << "Connected to PostgreSQL!";
        initTables();
    }
}

DatabaseManager::~DatabaseManager() {
    db.close();
}

void DatabaseManager::initTables() {
    QSqlQuery query;
    // Таблица users
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS users (
            id SERIAL PRIMARY KEY,
            telegram_id BIGINT NOT NULL UNIQUE,
            username TEXT,
            first_name TEXT,
            last_name TEXT,
            created_at TIMESTAMP DEFAULT NOW()
        )
    )")) {
        qDebug() << "Failed to create users table:" << query.lastError().text();
    }
    // Таблица user_survey_responses
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS user_survey_responses (
            id SERIAL PRIMARY KEY,
            user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,
            survey_id INTEGER NOT NULL,
            answer TEXT,
            answered_at TIMESTAMP DEFAULT NOW()
        )
    )")) {
        qDebug() << "Failed to create user_survey_responses table:" << query.lastError().text();
    }
}

bool DatabaseManager::addUser(qint64 telegramId, const QString& username, const QString& message) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (telegram_id, username) VALUES (:id, :username) ON CONFLICT (telegram_id) DO NOTHING");
    query.bindValue(":id", telegramId);
    query.bindValue(":username", username);
    
    if (!query.exec()) {
        qDebug() << "Failed to insert user:" << query.lastError().text();
        return false;
    }
    bool inserted = query.numRowsAffected() > 0;
    qDebug() << (inserted ? "User inserted successfully" : "User already exists, not inserted");
    return inserted;
}