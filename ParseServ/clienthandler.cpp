#include "ClientHandler.h"
#include <QDebug>

ClientHandler::ClientHandler(QTcpSocket* socket, QObject* parent)
    : QObject(parent), socket(socket) {
    connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);
}

void ClientHandler::onReadyRead() {
    QByteArray data = socket->readAll();
    QString message(data);
    qInfo() << "Received:" << message;
    processRequest(message);
}

void ClientHandler::onDisconnected() {
    socket->deleteLater();
    delete this;
}

void ClientHandler::processRequest(const QString& data) {
    // Пример формата запроса: "A,B,key"
    QStringList parts = data.split(",");
    if (parts.size() != 3) {
        socket->write("ERROR: Invalid request format.\n");
        return;
    }

    QString from = parts[0].trimmed();
    QString to = parts[1].trimmed();
    QString key = parts[2].trimmed();

    int distance = graphProcessor.findShortestPath(from, to);
    QString response = "Distance: " + QString::number(distance);
    QString encrypted = graphProcessor.vigenereEncrypt(response, key);

    socket->write(encrypted.toUtf8());
}
