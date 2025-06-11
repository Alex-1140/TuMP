#include "test_register.h"
#include "test_common.h"
#include <QDebug>

void testRegisterEmptyPassword() {
    QString response = sendCommand("REGISTER user1 ");
    if (response.contains("ERROR"))
        qInfo() << "[PASS] Registration without password rejected";
    else
        qCritical() << "[FAIL] Registration without password succeeded: " << response;
}
