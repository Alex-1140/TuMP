#include <QCoreApplication>
#include <QTcpSocket>
#include <QTextStream>
#include <iostream>

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    QTcpSocket socket;

    QTextStream in(stdin);
    std::cout << "Введите порт сервера: ";
    quint16 port;
    std::cin >> port;

    socket.connectToHost("localhost", port);
    if (!socket.waitForConnected(3000)) {
        std::cerr << "Не удалось подключиться: " << qPrintable(socket.errorString()) << std::endl;
        return 1;
    }

    std::cout << "Подключено! Вводите команды (EXIT для выхода):\n";

    QObject::connect(&socket, &QTcpSocket::readyRead, [&]() {
        QDataStream in(&socket);
        in.setVersion(QDataStream::Qt_6_0);
        QString response;
        in >> response;
        std::cout << qPrintable(response);
        std::cout.flush();
    });

    while (true) {
        std::string command;
        std::cout << "> ";
        std::getline(std::cin, command);
        if (command == "EXIT") break;

        QDataStream out(&socket);
        out.setVersion(QDataStream::Qt_6_0);
        out << QString::fromStdString(command);
        socket.flush();
    }

    socket.disconnectFromHost();
    return app.exec();
}
