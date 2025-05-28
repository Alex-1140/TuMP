#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QDebug>

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
}

DatabaseManager::~DatabaseManager() {
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::connect(const QString& path) {
    db.setDatabaseName(path);
    if (!db.open()) {
        qCritical() << "Failed to connect to database.";
        return false;
    }
    return true;
}
