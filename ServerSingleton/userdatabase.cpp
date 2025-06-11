#include "userdatabase.h"
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlerror.h>
#include <QDebug>

UserDatabase::~UserDatabase() {
    if (db.isOpen()) db.close();
}

UserDatabase& UserDatabase::getInstance() {
    static UserDatabase instance;
    return instance;
}

bool UserDatabase::init(const QString& path) {
    db = QSqlDatabase::addDatabase("QSQLITE", "user_connection");
    db.setDatabaseName(path);
    if (!db.open()) {
        qCritical() << "[UserDB] Cannot open DB:" << db.lastError();
        return false;
    }
    QSqlQuery query(db);
    return query.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, login TEXT UNIQUE, password TEXT)");
}

bool UserDatabase::addUser(const QString& login, const QString& password) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (login, password) VALUES (:login, :password)");
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    return query.exec();
}

bool UserDatabase::checkUser(const QString& login, const QString& password) {
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE login = :login AND password = :password");
    query.bindValue(":login", login);
    query.bindValue(":password", password);
    if (!query.exec()) return false;
    if (query.next()) return query.value(0).toInt() > 0;
    return false;
}
