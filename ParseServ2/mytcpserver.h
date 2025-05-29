#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>
#include <QVector>
#include <limits>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit MyTcpServer(QObject *parent = nullptr);
    void startServer();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void readClientData();
    void clientDisconnected();

private:
    QMap<int, QMap<int, double>> graph; // Граф: u -> {v -> вес}

    QMap<int, double> findShortestPaths(int src);
    QVector<int> getUniqueNodes();
    void sendWelcomeMessage(QTcpSocket *socket);
    void processCommand(QTcpSocket *socket, const QString &commandLine);
};

#endif // MYTCPSERVER_H
