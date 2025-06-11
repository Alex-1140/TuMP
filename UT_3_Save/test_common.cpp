#include "test_common.h"
#include "test_config.h"
#include <QtNetwork/qtcpsocket.h>
#include <QDataStream>

QString sendCommand(const QString& cmd) {
    QTcpSocket socket;
    socket.connectToHost(SERVER_HOST, SERVER_PORT);
    if (!socket.waitForConnected(2000)) return "ERROR: Connection failed";

    QDataStream out(&socket);
    out.setVersion(QDataStream::Qt_6_0);
    out << cmd;
    socket.flush();

    if (!socket.waitForReadyRead(2000)) return "ERROR: No response";
    QDataStream in(&socket);
    in.setVersion(QDataStream::Qt_6_0);
    QString response;
    in >> response;

    socket.disconnectFromHost();
    return response;
}
