#pragma once

#include <unordered_map>
#include <map>
#include <vector>
#include <string>
#include <mutex>
#include <limits>
#include <iostream>

using Vertex = int;
using Weight = double;
using AdjList = std::unordered_map<Vertex, std::vector<std::pair<Vertex, Weight>>>;

class DBServer {
public:
    // Получение единственного экземпляра класса (синглтон)
    static DBServer& getInstance();

    // Удаление копирования и присваивания (синглтон)
    DBServer(const DBServer&) = delete;
    DBServer& operator=(const DBServer&) = delete;

    // Операции с графом
    // Добавляет ребро в граф между вершинами u и v с весом w
    void addEdge(Vertex u, Vertex v, Weight w);
    
    // Вычисление кратчайшего пути от вершины src
    // до всех остальных вершин в графе с помощью
    // алгоритма Дейкстры
    std::vector<Weight> shortestPath(Vertex src);

    // Шифрование текста с использованием ключа (метод Виженера)
    static std::string vigenereEncrypt(const std::string&, const std::string&);
    
    // Сохранение/загрузка графа
    bool saveDatabase(const std::string& filename);
    bool loadDatabase(const std::string& filename);

    // Работа с шифром Виженера
    // Сохранение половины ключа и текста, вычисляя
    // зашифрованный результат для последующего использования
    void storeHalfKey(const std::string& half_key,
        const std::string& text,
        const std::string& result);
    
    // Получение сохраненного результата шифрования для заданной
    // половины ключа и текста
    bool getHalfKey(const std::string& half_key,
        const std::string& text,
        std::string& out);

    // Debug / Info
    // Выводит данные в переданный поток (по умолчанию — std::cout)
    void printData(std::ostream& os = std::cout);

    // Конфигурационный файл пользователей
    static constexpr const char* USERS_CFG = "users.cfg";

    // Регистрация и авторизация
    // Регистрирует нового пользователя
    bool registerUser(const std::string&, const std::string&);

    // Авторизация (проверка логин и пароль пользователя)
    bool loginUser(const std::string&, const std::string&);

    // Загрузка/сохранение пользователей в/из файла users.cfg
    void loadUsers();
    void saveUsers();

private:
    // Конструктор и деструктор для реализации синглтона
    DBServer();
    ~DBServer();

    // Список смежности для хранения графа
    AdjList graph_;
    
    // Хранилище половин ключей и результатов Виженера
    std::map<std::pair<std::string, std::string>, std::string> vigenere_halves_;
    
    // Хранилище пользователей (логин -> зашифрованный пароль)
    std::map<std::string, std::string> users_;
    
    // Обеспечение потокобезопасности
    std::mutex mutex_;
};