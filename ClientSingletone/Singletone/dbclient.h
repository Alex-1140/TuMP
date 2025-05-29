#pragma once

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QAbstractSocket>
#include <QString>

class DbClient : public QObject {
    Q_OBJECT
public:
    explicit DbClient(QObject* parent = nullptr);
    ~DbClient();

    void connectToServer(const QString& host, quint16 port);
    void sendCommand(const QString& command);
    void disconnectFromServer();

signals:
    void responseReceived(const QString& response);
    void errorOccurred(const QString& error);

private slots:
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError /*e*/);

private:
    QTcpSocket* socket_;
};
