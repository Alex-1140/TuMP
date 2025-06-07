#include "test_login.h"
#include "test_common.h"
#include <QCoreApplication>
#include <iostream>

bool testLoginIncomplete() {
    std::cout << "Test LOGIN without password... ";
    QString resp = sendCommand("LOGIN onlyusername");
    if (resp.startsWith("ERROR")) {
        std::cout << "PASS\n";
        return true;
    }
    else {
        std::cout << "FAIL (got \"" << resp.toStdString() << "\")\n";
        return false;
    }
}
