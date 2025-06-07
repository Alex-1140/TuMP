#include "test_save_db.h"
#include "test_common.h"

#include <QFile>
#include <QTextStream>
#include <QThread>
#include <iostream>

bool testPasswordEncryption(const QString& usersCfgPath) {
    std::cout << "Test password encryption in users.cfg... ";

    // задается путь по умолчанию, если он пуст
    QString cfg = usersCfgPath.isEmpty() ? QStringLiteral("users.cfg") : usersCfgPath;

    // чтение бэкапа
    QByteArray backup;
    {
        QFile f(cfg);
        if (f.open(QIODevice::ReadOnly)) {
            backup = f.readAll();
            f.close();
        }
    }

    const QString user = "autoTestUser";
    const QString pass = "simplePassword";

    sendCommand(QString("REGISTER %1 %2").arg(user, pass));

    QFile f(cfg);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "FAIL (cannot open " << cfg.toStdString() << ")\n";
        // восстановление
        if (!backup.isEmpty()) {
            QFile o(cfg);
            if (o.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                o.write(backup);
                o.close();
            }
        }
        return false;
    }

    bool found = false, hashed = false;
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith(user + ' ')) {
            found = true;
            QString stored = line.section(' ', 1);
            if (stored != pass) hashed = true;
            break;
        }
    }
    f.close();

    // восстановление
    if (!backup.isEmpty()) {
        QFile o(cfg);
        if (o.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            o.write(backup);
            o.close();
        }
    }

    if (!found) {
        std::cout << "FAIL (user not found in " << cfg.toStdString() << ")\n";
        return false;
    }
    if (hashed) {
        std::cout << "PASS\n";
        return true;
    }
    else {
        std::cout << "FAIL (password stored in plaintext)\n";
        return false;
    }
}
