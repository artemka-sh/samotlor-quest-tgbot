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
    } else {
        qDebug() << "Table 'users' checked/created.";
    }
    // Таблица user_answers
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS user_answers (
            id SERIAL PRIMARY KEY,
            user_id INTEGER REFERENCES users(id) ON DELETE CASCADE,
            question_id INTEGER NOT NULL,
            type TEXT NOT NULL,
            answer TEXT,
            answered_at TIMESTAMP DEFAULT NOW()
        )
    )")) {
        qDebug() << "Failed to create user_answers table:" << query.lastError().text();
    } else {
        qDebug() << "Table 'user_answers' checked/created.";
    }
}

bool DatabaseManager::addUser(qint64 telegramId, const QString& username, const QString& firstName, const QString& lastName) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (telegram_id, username, first_name, last_name) VALUES (:id, :username, :first_name, :last_name) ON CONFLICT (telegram_id) DO NOTHING");
    query.bindValue(":id", telegramId);
    query.bindValue(":username", username);
    query.bindValue(":first_name", firstName);
    query.bindValue(":last_name", lastName);
    bool ok = query.exec();
    if (!ok) {
        qDebug() << "[addUser] Failed to insert user:" << query.lastError().text();
        return false;
    }
    bool inserted = query.numRowsAffected() > 0;
    qDebug() << (inserted ? "[addUser] User inserted successfully" : "[addUser] User already exists, not inserted") << "telegram_id:" << telegramId;
    return inserted;
}

bool DatabaseManager::hasUser(qint64 telegramId) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE telegram_id = :id");
    query.bindValue(":id", telegramId);
    bool ok = query.exec();
    if (!ok) {
        qDebug() << "[hasUser] Query failed:" << query.lastError().text();
        return false;
    }
    bool exists = query.next();
    qDebug() << "[hasUser] User exists:" << exists << "for telegram_id:" << telegramId;
    return exists;
}

int DatabaseManager::getUserId(qint64 telegramId) {
    QSqlQuery userQuery;
    userQuery.prepare("SELECT id FROM users WHERE telegram_id = :id");
    userQuery.bindValue(":id", telegramId);
    if (!userQuery.exec() || !userQuery.next()) {
        qDebug() << "[getUserId] User not found for telegram_id:" << telegramId;
        return -1;
    }
    return userQuery.value(0).toInt();
}

void DatabaseManager::saveAnswer(qint64 telegramId, int questionId, const std::string& answer, const std::string& type) {
    int userId = getUserId(telegramId);
    if (userId == -1) {
        qDebug() << "[saveAnswer] User not found for telegram_id:" << telegramId;
        return;
    }
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO user_answers (user_id, question_id, type, answer)
        VALUES (:user_id, :question_id, :type, :answer)
    )");
    query.bindValue(":user_id", userId);
    query.bindValue(":question_id", questionId);
    query.bindValue(":type", QString::fromStdString(type));
    query.bindValue(":answer", QString::fromStdString(answer));
    if (!query.exec()) {
        qDebug() << "[saveAnswer] Failed to insert answer:" << query.lastError().text();
    } else {
        qDebug() << "[saveAnswer] Answer saved for user_id:" << userId << "question_id:" << questionId << "type:" << QString::fromStdString(type);
    }
}

void DatabaseManager::updateAnswer(qint64 telegramId, int questionId, const std::string& answer, const std::string& type) {
    int userId = getUserId(telegramId);
    if (userId == -1) {
        qDebug() << "[updateAnswer] User not found for telegram_id:" << telegramId;
        return;
    }
    QSqlQuery query;
    query.prepare(R"(
        UPDATE user_answers
        SET answer = :answer, type = :type, answered_at = NOW()
        WHERE user_id = :user_id AND question_id = :question_id
    )");
    query.bindValue(":user_id", userId);
    query.bindValue(":question_id", questionId);
    query.bindValue(":type", QString::fromStdString(type));
    query.bindValue(":answer", QString::fromStdString(answer));
    if (!query.exec()) {
        qDebug() << "[updateAnswer] Failed to update answer:" << query.lastError().text();
    } else {
        qDebug() << "[updateAnswer] Answer updated for user_id:" << userId << "question_id:" << questionId;
    }
}

int DatabaseManager::getLastAnsweredQuestionId(qint64 telegramId) {
    int userId = getUserId(telegramId);
    if (userId == -1) {
        qDebug() << "[getLastAnsweredQuestionId] User not found for telegram_id:" << telegramId;
        return -1;
    }
    QSqlQuery query;
    query.prepare(R"(
        SELECT question_id
        FROM user_answers
        WHERE user_id = :user_id
        ORDER BY answered_at DESC, id DESC
        LIMIT 1
    )");
    query.bindValue(":user_id", userId);
    if (!query.exec() || !query.next()) {
        qDebug() << "[getLastAnsweredQuestionId] No answers found for user_id:" << userId;
        qDebug() << "[getLastAnsweredQuestionId] Return default id";
        return -1;
    }
    int qid = query.value(0).toInt();
    qDebug() << "[getLastAnsweredQuestionId] Last answered question_id:" << qid << "for user_id:" << userId;
    return qid;
}

bool DatabaseManager::isQuestionAnswered(qint64 telegramId, int questionId) {
    int userId = getUserId(telegramId);
    if (userId == -1) {
        qDebug() << "[isQuestionAnswered] User not found for telegram_id:" << telegramId;
        return false;
    }
    QSqlQuery query;
    query.prepare(R"(
        SELECT answer FROM user_answers
        WHERE user_id = :user_id AND question_id = :question_id
        LIMIT 1
    )");
    query.bindValue(":user_id", userId);
    query.bindValue(":question_id", questionId);
    if (!query.exec() || !query.next()) {
        return false;
    }
    QString answer = query.value(0).toString();
    return !answer.isEmpty();
}