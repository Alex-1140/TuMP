#ifndef VIGENERECIPHERTESTS_H
#define VIGENERECIPHERTESTS_H

#include <QObject>
#include <QTest>
#include "../ParseServer/Server/server.h"  // Путь к вашему серверу

class VigenereCipherTests : public QObject
{
    Q_OBJECT
public:
    VigenereCipherTests();
    ~VigenereCipherTests();
private:
    Server* server;

private slots:
    void initTestCase(); // Вызывается перед первым тестом
    void cleanupTestCase(); // Вызывается после последнего теста
    void testBasicEncryption();
    void testBasicDecryption();
    void testMixedCase();
    void testNonAlphabeticChars();
    void testEmptyString();
};

#endif // VIGENERECIPHERTESTS_H
