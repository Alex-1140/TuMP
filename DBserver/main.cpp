#include "DBServer.h"
#include <QCoreApplication>
#include <iostream>
#include <clocale>

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");
    QCoreApplication app(argc, argv);

    quint16 port;
    std::cout << "Введите порт для сервера: ";
    std::cin >> port;

    DBServer server(port);
    if (!server.isListening()) {
        std::cerr << "Не удалось запустить сервер. Завершение работы.\n";
        return 1;
    }

    std::cout << "Сервер запущен. Ожидание подключений...\n";

    return app.exec();
}
