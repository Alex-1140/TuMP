#include "client.h"
#include <iostream>

Client::Client(QObject* parent) : QObject(parent) {
    std::cout << "Attempting to connect to server..." << std::endl;
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, []() {
        std::cout << "Connected to server!" << std::endl;
    });
    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError socketError) {
        std::cout << "Connection error: " << socketError << " - " << this->socket->errorString().toStdString() << std::endl;
    });
    connect(socket, &QTcpSocket::readyRead, this, &Client::readResponse);
    connect(socket, &QTcpSocket::disconnected, this, []() {
        std::cout << "Disconnected from server" << std::endl;
    });
    socket->connectToHost("127.0.0.1", 12345);
    std::cout << "Connection attempt initiated" << std::endl;
    if (!socket->waitForConnected(5000)) {
        std::cout << "Failed to connect to server: " << socket->errorString().toStdString() << std::endl;
    }
}

void Client::sendCommand(const QString& command) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        std::cout << "Sending command: " << command.toStdString() << std::endl;
        QDataStream out(socket);
        out.setVersion(QDataStream::Qt_5_15);
        out << command;
        socket->flush();
        std::cout << "Command sent, bytes written: " << socket->bytesToWrite() << std::endl;
        // Ожидаем ответа от сервера
        if (!socket->waitForReadyRead(3000)) {
            std::cout << "No response received within 3 seconds: " << socket->errorString().toStdString() << std::endl;
        }
    } else {
        std::cout << "Not connected to server!" << std::endl;
    }
}

void Client::readResponse() {
    std::cout << "Reading response... Available bytes: " << socket->bytesAvailable() << std::endl;
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_15);
    QString response;
    in >> response;

    // Определяем статус выполнения команды
    QString status = "Failed"; // По умолчанию считаем команду неуспешной
    if (response.startsWith("OK") || response.startsWith("RESULT") ||
        response.startsWith("DISTANCES") || response.startsWith("STRING")) {
        status = "Success";
    } else if (response.startsWith("ERROR")) {
        status = "Failed";
    }

    // Выводим ответ сервера и статус
    std::cout << "Server response: " << response.toStdString() << " | Status: " << status.toStdString() << std::endl;
    std::cout << "Bytes remaining: " << socket->bytesAvailable() << std::endl;
}
