#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QCryptographicHash>
#include "graph.h"
#include "database.h"

class Server : public QObject {
    Q_OBJECT
public:
    explicit Server(QObject* parent = nullptr);
    QString vigenereCipher(const QString& text, bool encrypt);
    QString hashWithSha512(const QString& data);

private slots:
    void handleNewConnection();

private:
    QTcpServer* tcpServer;
    Graph graph;
    Database db;
    QString vigenereKey = "KEY";
    QString storedKey;
    QMap<QTcpSocket*, QString> authenticatedUsers;
    QString usersFile = "users.conf";

    bool isAuthenticated(QTcpSocket* socket);
    void saveUser(const QString& login, const QString& password);
    bool checkUser(const QString& login, const QString& password);

    double evaluateFunction(const QString& expr, double x); // Новый метод для вычисления f(x)
    QString halfMethod(double a, double b, double epsilon, const QString& expr); // Метод деления пополам
};

#endif // SERVER_H
