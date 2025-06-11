// dbclient.cpp
#include "dbclient.h"
#include <QTextStream>

DbClient::DbClient(QObject* parent)
    : QObject(parent), socket_(new QTcpSocket(this)) {
    connect(socket_, &QTcpSocket::readyRead, this, &DbClient::onReadyRead);
    connect(socket_, &QTcpSocket::errorOccurred, this, &DbClient::onErrorOccurred);
    connect(socket_, &QTcpSocket::connected, this, &DbClient::connected);
}

DbClient::~DbClient() {
    disconnectFromServer();
}

void DbClient::connectToServer(const QString& host, quint16 port) {
    socket_->connectToHost(host, port);
}

void DbClient::sendCommand(const QString& command) {
    if (!socket_ || !socket_->isOpen()) {
        emit errorOccurred("Not connected to server.");
        return;
    }

    QDataStream out(socket_);
    out.setVersion(QDataStream::Qt_6_0); // убедись, что версия одинакова с сервером
    out << command;
}

void DbClient::disconnectFromServer() {
    socket_->disconnectFromHost();
}

void DbClient::onReadyRead() {
    QDataStream in(socket_);
    in.setVersion(QDataStream::Qt_6_0);
    QString response;
    while (!in.atEnd()) {
        in >> response;
        emit responseReceived(response);
    }
}

void DbClient::onErrorOccurred(QAbstractSocket::SocketError) {
    emit errorOccurred(socket_->errorString());
}
