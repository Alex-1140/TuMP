#ifndef USERDATABASE_H
#define USERDATABASE_H

#include <QString>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqlerror.h>

class UserDatabase {
public:
    static UserDatabase& getInstance();
    bool init(const QString& path);
    bool addUser(const QString& login, const QString& password);
    bool checkUser(const QString& login, const QString& password);

private:
    UserDatabase() = default;
    ~UserDatabase();
    QSqlDatabase db;
    UserDatabase(const UserDatabase&) = delete;
    UserDatabase& operator=(const UserDatabase&) = delete;
};

#endif // USERDATABASE_H