#include "dbclient.h"
#include <QtCore/QTextStream>

DbClient::DbClient(QObject* parent)
    : QObject(parent), socket_(new QTcpSocket(this))
{
    connect(socket_, &QTcpSocket::readyRead, this, &DbClient::onReadyRead);
    connect(socket_, &QTcpSocket::errorOccurred, this, &DbClient::onErrorOccurred);
}

DbClient::~DbClient() {
    disconnectFromServer();
}

void DbClient::connectToServer(const QString& host, quint16 port) {
    socket_->connectToHost(host, port);
    connect(socket_, &QTcpSocket::connected, this, []() {
        qDebug() << "[DbClient] Connected to server";
        });
}

void DbClient::sendCommand(const QString& command) {
    if (socket_->state() == QAbstractSocket::ConnectedState) {
        QByteArray data = command.toUtf8();
        if (!data.endsWith('\n')) data.append('\n');
        socket_->write(data);
    }
    else {
        emit errorOccurred("Not connected");
    }
}

void DbClient::disconnectFromServer() {
    socket_->disconnectFromHost();
}

void DbClient::onReadyRead() {
    while (socket_->canReadLine()) {
        QByteArray line = socket_->readLine();
        QString response = QString::fromLocal8Bit(line).trimmed();
        emit responseReceived(response);
    }
}

void DbClient::onErrorOccurred(QAbstractSocket::SocketError) {
    emit errorOccurred(socket_->errorString());
}
