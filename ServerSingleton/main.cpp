
#include <QCoreApplication>
#include "server.h"
#include "userdatabase.h"

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    if (!UserDatabase::getInstance().init("users.db")) {
        qCritical() << "[main] Failed to initialize user database";
        return 1;
    }

    Server server;
    return a.exec();
}