#pragma once

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QAbstractSocket>
#include <QString>

// Параметры сервера
static constexpr const char* SERVER_IP = "127.0.0.1";
static constexpr quint16      SERVER_PORT = 8080;

// Посылает одну строку (command+'\n') на сервер, ждёт одну строчку-ответ.
// При ошибке возвращает "ERROR: …"
inline QString sendCommand(const QString& command) {
    QTcpSocket sock;
    sock.connectToHost(QHostAddress(SERVER_IP), SERVER_PORT);
    if (!sock.waitForConnected(3000))
        return QStringLiteral("ERROR: Cannot connect (%1)").arg(sock.errorString());

    QString cmd = command;
    if (!cmd.endsWith('\n')) cmd.append('\n');
    auto data = cmd.toUtf8();
    if (sock.write(data) == -1 || !sock.waitForBytesWritten(2000)) {
        sock.disconnectFromHost();
        return QStringLiteral("ERROR: Write timeout (%1)").arg(sock.errorString());
    }
    if (!sock.waitForReadyRead(3000)) {
        sock.disconnectFromHost();
        return QStringLiteral("ERROR: Read timeout");
    }

    auto line = sock.readLine().trimmed();
    sock.disconnectFromHost();
    return QString::fromLocal8Bit(line);
}
