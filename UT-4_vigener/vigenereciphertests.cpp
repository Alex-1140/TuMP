#include "vigenereciphertests.h"



void VigenereCipherTests::testBasicEncryption()
{
    QString plaintext = "HELLO";
    QString encrypted = server->vigenereCipher(plaintext, true);
    QCOMPARE(encrypted, QString("RIJVS"));
}

void VigenereCipherTests::testBasicDecryption()
{
    QString ciphertext = "RIJVS";
    QString decrypted = server->vigenereCipher(ciphertext, false);
    QCOMPARE(decrypted, QString("HELLO"));
}

void VigenereCipherTests::testMixedCase()
{
    QString plaintext = "Hello World";
    QString encrypted = server->vigenereCipher(plaintext, true);
    QCOMPARE(encrypted, QString("Rijvs Uyvjn"));

    QString decrypted = server->vigenereCipher(encrypted, false);
    QCOMPARE(decrypted, plaintext);
}

void VigenereCipherTests::testNonAlphabeticChars()
{
    QString plaintext = "Hello, World! 123";
    QString encrypted = server->vigenereCipher(plaintext, true);
    QCOMPARE(encrypted, QString("Rijvs, Uyvjn! 123"));

    QString decrypted = server->vigenereCipher(encrypted, false);
    QCOMPARE(decrypted, plaintext);
}

void VigenereCipherTests::testEmptyString()
{
    QString plaintext = "";
    QString encrypted = server->vigenereCipher(plaintext, true);
    QVERIFY(encrypted.isEmpty());

    QString decrypted = server->vigenereCipher(encrypted, false);
    QVERIFY(decrypted.isEmpty());
}

VigenereCipherTests::VigenereCipherTests()
{
    server = new Server(nullptr); // Явная инициализация
}

VigenereCipherTests::~VigenereCipherTests()
{
    delete server; // Не забываем освобождать память
}

void VigenereCipherTests::initTestCase()
{
    qDebug() << "Начало тестирования VigenereCipher...";
}

void VigenereCipherTests::cleanupTestCase()
{
    qDebug() << "Тестирование VigenereCipher завершено";
}
