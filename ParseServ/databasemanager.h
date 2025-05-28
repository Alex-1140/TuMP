#pragma once
#include <QString>
#include <QSqlDatabase>

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    bool connect(const QString& path);
    // Добавить методы, например:
    // bool saveResult(...);
    // QStringList getVertices();
private:
    QSqlDatabase db;
};
