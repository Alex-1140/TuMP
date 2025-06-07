#include "test_register.h"
#include "test_common.h"
#include <QCoreApplication>
#include <iostream>

bool testRegisterIncomplete() {
    std::cout << "Test 1: REGISTER without password... ";
    QString resp = sendCommand("REGISTER onlyusername");
    if (resp.startsWith("ERROR")) {
        std::cout << "PASS\n";
        return true;
    }
    else {
        std::cout << "FAIL (got \"" << resp.toStdString() << "\")\n";
        return false;
    }
}