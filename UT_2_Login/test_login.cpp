#include "test_login.h"
#include "test_common.h"
#include <QDebug>

void testLoginEmptyPassword() {
    QString response = sendCommand("LOGIN user1 ");
    if (response.contains("ERROR"))
        qInfo() << "[PASS] Login without password rejected";
    else
        qCritical() << "[FAIL] Login without password succeeded: " << response;
}
