#include <QCoreApplication>
#include <QTcpServer>
#include "ClientHandler.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    QTcpServer server;
    if (!server.listen(QHostAddress::Any, 12345)) {
        qCritical() << "Server could not start!";
        return 1;
    }
    qInfo() << "Server started on port 12345.";

    QObject::connect(&server, &QTcpServer::newConnection, [&]() {
        QTcpSocket *clientSocket = server.nextPendingConnection();
        new ClientHandler(clientSocket);
    });

    return a.exec();
}
