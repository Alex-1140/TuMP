#include <QCoreApplication>
#include "test_register.h" // или test_login.h, и т.д.

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);

    testRegisterEmptyPassword(); // или testLoginEmptyPassword() и т.д.

    return 0;
}
