#include "test_get_key.h"
#include "test_common.h"
#include <QCoreApplication>
#include <QThread>
#include <iostream>

bool testGetKey() {
    std::cout << "Test GET_KEY returns hash... ";

    const QString halfKey = "halfKey123";
    const QString text = "SomeSecretText";

    // STORE_KEY
    QString r1 = sendCommand(QString("STORE_KEY %1 %2 -").arg(halfKey, text));
    if (!r1.startsWith("OK")) {
        std::cout << "FAIL (STORE_KEY -> \"" << r1.toStdString() << "\")\n";
        return false;
    }
    QThread::msleep(200);

    // GET_KEY
    QString r2 = sendCommand(QString("GET_KEY %1 %2").arg(halfKey, text));
    if (r2.startsWith("RESULT: ")) {
        QString h = r2.section(' ', 1);
        bool ok = (h.length() >= 16);
        for (auto c : h) if (!c.isLetterOrNumber()) { ok = false; break; }
        if (ok) {
            std::cout << "PASS\n";
            return true;
        }
    }
    std::cout << "FAIL (GET_KEY -> \"" << r2.toStdString() << "\")\n";
    return false;
}
