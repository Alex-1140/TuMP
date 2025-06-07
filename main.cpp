#include <QCoreApplication>
#include <QString>
#include <iostream>

#include "test_register.h"
#include "test_login.h"
#include "test_save_db.h"
#include "test_get_key.h"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    // 1) Указать путь к users.cfg
    std::cout << "Enter full path to users.cfg (or leave empty for './users.cfg'): ";
    std::string line;
    std::getline(std::cin, line);
    QString usersCfgPath = QString::fromStdString(line);

    int passed = 0, total = 4;
    std::cout << "\n=== Running all tests ===\n";

    std::cout << "\n1) REGISTER without password:\n";
    if (testRegisterIncomplete()) passed++;

    std::cout << "\n2) LOGIN without password:\n";
    if (testLoginIncomplete()) passed++;

    std::cout << "\n3) Password encryption in users.cfg:\n";
    if (testPasswordEncryption(usersCfgPath)) passed++;

    std::cout << "\n4) GET_KEY returns hash:\n";
    if (testGetKey()) passed++;

    std::cout << "\n=== Summary: " << passed
        << " / " << total << " tests passed ===\n";

    return (passed == total) ? 0 : 1;
}
