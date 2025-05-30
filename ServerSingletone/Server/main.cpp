#include "dbserver.h" // интерфейс DBServer (синглтон)
#include <thread> // для многопотока
#include <string>
#include <sstream>
#include <iostream>

// Многопоточность в Win и UNIX системах
// Многопоточность нужна для достижения высокой производительности приложения
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
using socklen_t = int;
using ssize_t = int;  // определяем ssize_t для Windows
#define CLOSESOCKET closesocket
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define CLOSESOCKET close
#endif

// Порты и параметры очереди прослушивания
constexpr int PORT = 8080;
constexpr int BACKLOG = 10;
constexpr size_t BUFFER_SIZE = 1024;

// Обработка запросов от клиента в отдельном потоке
// Принимает все входящие комманды, и шлет ответы
void handleClient(int clientSock) {
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    // Получение экземпляра сервера (синглтон)
    auto& db = DBServer::getInstance();
    // Обязательная подгрузка базы пользователей для аутентификации
    db.loadUsers();

    // Чтение из сокета во время активного соединения
    while ((bytesRead = recv(clientSock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';
        std::istringstream iss(buffer);
        std::string cmd;
        iss >> cmd;
        std::ostringstream response;

        // Добавление ребра в граф
        if (cmd == "ADD_EDGE") {
            int u, v;
            double w;
            iss >> u >> v >> w;
            db.addEdge(u, v, w);
            response << "OK: Edge added " << u << "-" << v << " w=" << w << "\n";
        }
        // Поиск кратчайшего пути
        else if (cmd == "SHORTEST_PATH") {
            int src;
            iss >> src;
            auto dist = db.shortestPath(src);
            response << "DISTANCES from " << src << ":";
            for (size_t i = 0; i < dist.size(); ++i) {
                if (dist[i] < std::numeric_limits<Weight>::infinity())
                    response << " [" << i << ":" << dist[i] << "]";
            }
            response << "\n";
        }
        // Сохранение данных в файл
        else if (cmd == "SAVE_DB") {
            std::string filename;
            iss >> filename;
            bool ok = db.saveDatabase(filename);
            response << (ok ? "OK: Database saved\n" : "ERROR: Save failed\n");
        }
        // Загрузка данных из файла
        else if (cmd == "LOAD_DB") {
            std::string filename;
            iss >> filename;
            bool ok = db.loadDatabase(filename);
            response << (ok ? "OK: Database loaded\n" : "ERROR: Load failed\n");
        }
        // Работа с шифром Виженера — хранение ключа
        else if (cmd == "STORE_KEY") {
            std::string half, text, result;
            iss >> half >> text >> result;
            db.storeHalfKey(half, text, result);
            response << "OK: Key stored\n";
        }
        // Работа с шифром Виженера — получение ключа
        else if (cmd == "GET_KEY") {
            std::string half, text;
            iss >> half >> text;
            std::string out;
            if (db.getHalfKey(half, text, out)) {
                response << "RESULT: " << out << "\n";
            }
            else {
                response << "ERROR: Key not found\n";
            }
        }
        // Вывод данных
        else if (cmd == "PRINT_DATA") {
            std::ostringstream oss;
            db.printData(oss);
            response << oss.str();
        }
        // Регистрация нового пользователя с учетом SHA-512
        else if (cmd == "REGISTER") {
            std::string username, password;
            iss >> username >> password;
            if (db.registerUser(username, password)) {
                response << "REGISTERED\n";
            }
            else {
                response << "ERROR: Username already exists\n";
            }
        }
        // Авторизация пользователей
        else if (cmd == "LOGIN") {
            std::string username, password;
            iss >> username >> password;
            if (db.loginUser(username, password)) {
                response << "LOGIN OK\n";
            }
            else {
                response << "ERROR: Invalid login or password\n";
            }
        }
        // Справка
        else if (cmd == "HELP") {
            response << "Available commands:\n"
                << "CONNECT — установка TCP-соединения"
                << "REGISTER <user> <password>  — создание нового пользователя\n"
                << "LOGIN <user> <password> — авторизация пользователя\n"
                << "ADD_EDGE <u> <v> <w>    — добавление ребра в граф"
                << "SHORTEST_PATH <src> — поиск кратчайшего расстояния от вершины графа\n"
                << "SAVE_DB <file>  — сохранение графа в файл\n"
                << "LOAD_DB <file>  — загрузка графа из файла\n"
                << "STORE_KEY <halfkey> <text> <ignore> — зашифровать текст Виженером\n"
                << "GET_KEY <halfkey> <text>    — получить SHA-512, сохраненного по ключу\n"
                << "PRINT_DATA  — вывод всех сохраненных данных\n"
                << "HELP    — страница помощи\n"
                << "QUIT    — закрыть соединение\n";
        }
        // Разрыв соединения (сокет, сформированный ответ серверу)
        else if (cmd == "QUIT") {
            response << "BYE\n";
            send(clientSock, response.str().c_str(), (int)response.str().size(), 0);
            break;
        }
        // На случай несуществующей команды
        else {
            response << "ERROR: Unknown command\n";
        }
        send(clientSock, response.str().c_str(), (int)response.str().size(), 0);
    }
    // Закрытие сокета
    CLOSESOCKET(clientSock);
}

int main() {
#ifdef _WIN32
    // Инициализация Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
#endif

    // Инициализация класса и загрузка пользователей
    auto& db = DBServer::getInstance();

    // Создание серверного сокета
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        perror("socket");
        return 1;
    }

    std::cout << "Server started, users loaded from " << DBServer::USERS_CFG << "\n";

    // Быстрый рестарт сервера
    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    // Привязка сокета к IP и порту
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(serverSock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    // Прослушка входящих соединений
    if (listen(serverSock, BACKLOG) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    // Прием клиента
    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientSock = accept(serverSock, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        if (clientSock < 0) {
            perror("accept");
            continue; // При ошибке идет ожидание следующего клиента
        }
        std::thread(handleClient, clientSock).detach();
    }

    // Закрытие сокета
    CLOSESOCKET(serverSock);
#ifdef _WIN32
    WSACleanup();
#endif
    db.saveUsers();
    return 0;
}
