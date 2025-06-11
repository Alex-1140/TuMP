#include "test_save_db.h"
#include "test_common.h"
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlerror.h>
#include <QCryptographicHash>
#include <QFile>
#include <QDebug>
#include <QRandomGenerator>

void testPasswordHashStored() {
    // 1. Уникальные логин и пароль
    QString login = "hash_user_" + QString::number(QRandomGenerator::global()->bounded(100000));
    QString plainPassword = "mypass123";
    QString hashedPassword = QString(QCryptographicHash::hash(plainPassword.toUtf8(), QCryptographicHash::Sha512).toHex());

    // 2. Отправка команды регистрации на сервер
    QString registerCommand = QString("REGISTER %1 %2").arg(login, plainPassword);
    QString response = sendCommand(registerCommand);

    if (!response.contains("OK")) {
        qCritical() << "[FAIL] Registration failed: " << response;
        return;
    }

    // 3. УКАЗАННЫЙ ПУТЬ К БАЗЕ
    QString dbPath = "D:/TuMP/DBSingleton/users.db";  // <-- ИЗМЕНИТЬ путь под свой
    QFile dbFile(dbPath);
    if (!dbFile.exists()) {
        qCritical() << "[FAIL] users.db does not exist at: " << dbPath;
        return;
    }

    // 4. Подключение к базе данных
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "user_connection");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qCritical() << "[FAIL] Failed to open users.db: " << db.lastError().text();
        return;
    }

    // 5. Проверка записи о пользователе
    QSqlQuery query(db);
    query.prepare("SELECT password FROM users WHERE login = :login");
    query.bindValue(":login", login);

    if (!query.exec()) {
        qCritical() << "[FAIL] Query failed: " << query.lastError().text();
        db.close();
        return;
    }

    if (!query.next()) {
        qCritical() << "[FAIL] User not found in database.";
        db.close();
        return;
    }

    QString storedPassword = query.value(0).toString();
    db.close();

    if (storedPassword == plainPassword) {
        qCritical() << "[FAIL] Password stored as plain text!";
    }
    else if (storedPassword == hashedPassword) {
        qInfo() << "[PASS] Password stored as SHA-512 hash.";
    }
    else {
        qWarning() << "[WARN] Password hash mismatch. Stored:" << storedPassword;
    }
}