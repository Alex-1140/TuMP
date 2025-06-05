#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>

class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(QObject* parent = nullptr);
    void sendCommand(const QString& command);

private slots:
    void readResponse();

private:
    QTcpSocket* socket;
};

#endif // CLIENT_H
