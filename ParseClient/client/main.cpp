#include <QCoreApplication>
#include "client.h"
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[]) {
    std::cout << "Client main function started" << std::endl;
    QCoreApplication app(argc, argv);
    Client client;
    std::cout << "Client object created, starting command input" << std::endl;
    QTextStream stream(stdin);
    while (true) {
        QString command = stream.readLine();
        if (command.isEmpty()) continue;
        client.sendCommand(command);
        if (command == "QUIT") break;
    }
    return app.exec();
}
