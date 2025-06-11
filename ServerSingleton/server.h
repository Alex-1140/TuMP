#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>
#include <QMap>
#include <QCryptographicHash>
#include "graph.h"
#include "database.h"
#include "userdatabase.h"

class Server : public QObject {
    Q_OBJECT

public:
    explicit Server(QObject* parent = nullptr);

private slots:
    void handleNewConnection();

private:
    QTcpServer* tcpServer;
    QMap<QTcpSocket*, QString> authenticatedUsers;
    Graph graph;
    Database db;
    QString vigenereKey = "KEY";
    QString storedKey;

    bool isAuthenticated(QTcpSocket* socket);
    QString vigenereCipher(const QString& text, bool encrypt);
    QString hashWithSha512(const QString& data);
    double evaluateFunction(const QString& expr, double x);
    QString halfMethod(double a, double b, double epsilon, const QString& expr);
};

#endif // SERVER_H