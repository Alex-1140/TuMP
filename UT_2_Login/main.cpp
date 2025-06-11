#include <QCoreApplication>
#include "test_login.h"

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    testLoginEmptyPassword();

    return 0;
}
