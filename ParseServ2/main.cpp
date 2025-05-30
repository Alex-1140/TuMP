#include "mytcpserver.h"
#include <QCoreApplication>

    int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MyTcpServer server;
    server.startServer();

    return a.exec();
}

