#include <QCoreApplication>
#include <QTest>
#include <QDebug>
#include "vigenereciphertests.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    VigenereCipherTests tests;

    int result = QTest::qExec(&tests, argc, argv);

    if (result == 0) {
        qDebug() << "======================================";
        qDebug() << "Все тесты успешно пройдены!";
        qDebug() << "======================================";
    } else {
        qDebug() << "======================================";
        qDebug() << "Обнаружены ошибки в тестах!";
        qDebug() << "======================================";

    }
    return result;
}
/*
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    VigenereCipherTests tests;
    return QTest::qExec(&tests, argc, argv);
}
*/
