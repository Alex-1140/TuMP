#pragma once
#include <QString>
// Тест 1: Проверка хеш в файле конфигурации
// Возвращает true, если сервер корректно вернул ERROR

// Путь к users.cfg в качестве аргумента
bool testPasswordEncryption(const QString& usersCfgPath);
