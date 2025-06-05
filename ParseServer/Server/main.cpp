#include <QCoreApplication>
#include "server.h"
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[]) {
    std::cout << "Server main function started" << std::endl;
    QCoreApplication app(argc, argv);
    Server server;
    std::cout << "Server object created, entering event loop" << std::endl;
    return app.exec();
}
