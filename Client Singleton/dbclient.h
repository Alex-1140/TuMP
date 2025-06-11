// dbclient.h
#pragma once

#include <QObject>
#include <QtNetwork/QTcpSocket>

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
    void connected();

private slots:
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError);

private:
    QTcpSocket* socket_;
};