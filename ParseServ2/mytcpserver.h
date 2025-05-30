#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>
#include "server_db.h" // Только заголовочный файл

    class MyTcpServer : public QTcpServer {
    Q_OBJECT
public:
    MyTcpServer(QObject *parent = nullptr);
    void startServer();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void readClientData();
    void clientDisconnected();

private:
    QMap<int, QMap<int, double>> graph;
    QMap<QTcpSocket*, QString> authenticatedUsers;
    void sendWelcomeMessage(QTcpSocket *socket);
    void processCommand(QTcpSocket *socket, const QString &commandLine);
    QMap<int, double> findShortestPaths(int src);
    QVector<int> getUniqueNodes();
};

#endif // MYTCPSERVER_H
