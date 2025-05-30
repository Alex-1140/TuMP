#include "dbserver.h" // ��������� DBServer (��������)
#include <thread> // ��� �����������
#include <string>
#include <sstream>
#include <iostream>

// ��������������� � Win � UNIX ��������
// ��������������� ����� ��� ���������� ������� ������������������ ����������
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
using socklen_t = int;
using ssize_t = int;  // ���������� ssize_t ��� Windows
#define CLOSESOCKET closesocket
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#define CLOSESOCKET close
#endif

// ����� � ��������� ������� �������������
constexpr int PORT = 8080;
constexpr int BACKLOG = 10;
constexpr size_t BUFFER_SIZE = 1024;

// ��������� �������� �� ������� � ��������� ������
// ��������� ��� �������� ��������, � ���� ������
void handleClient(int clientSock) {
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    // ��������� ���������� ������� (��������)
    auto& db = DBServer::getInstance();
    // ������������ ��������� ���� ������������� ��� ��������������
    db.loadUsers();

    // ������ �� ������ �� ����� ��������� ����������
    while ((bytesRead = recv(clientSock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';
        std::istringstream iss(buffer);
        std::string cmd;
        iss >> cmd;
        std::ostringstream response;

        // ���������� ����� � ����
        if (cmd == "ADD_EDGE") {
            int u, v;
            double w;
            iss >> u >> v >> w;
            db.addEdge(u, v, w);
            response << "OK: Edge added " << u << "-" << v << " w=" << w << "\n";
        }
        // ����� ����������� ����
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
        // ���������� ������ � ����
        else if (cmd == "SAVE_DB") {
            std::string filename;
            iss >> filename;
            bool ok = db.saveDatabase(filename);
            response << (ok ? "OK: Database saved\n" : "ERROR: Save failed\n");
        }
        // �������� ������ �� �����
        else if (cmd == "LOAD_DB") {
            std::string filename;
            iss >> filename;
            bool ok = db.loadDatabase(filename);
            response << (ok ? "OK: Database loaded\n" : "ERROR: Load failed\n");
        }
        // ������ � ������ �������� � �������� �����
        else if (cmd == "STORE_KEY") {
            std::string half, text, result;
            iss >> half >> text >> result;
            db.storeHalfKey(half, text, result);
            response << "OK: Key stored\n";
        }
        // ������ � ������ �������� � ��������� �����
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
        // ����� ������
        else if (cmd == "PRINT_DATA") {
            std::ostringstream oss;
            db.printData(oss);
            response << oss.str();
        }
        // ����������� ������ ������������ � ������ SHA-512
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
        // ����������� �������������
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
        // �������
        else if (cmd == "HELP") {
            response << "Available commands:\n"
                << "CONNECT � ��������� TCP-����������"
                << "REGISTER <user> <password>  � �������� ������ ������������\n"
                << "LOGIN <user> <password> � ����������� ������������\n"
                << "ADD_EDGE <u> <v> <w>    � ���������� ����� � ����"
                << "SHORTEST_PATH <src> � ����� ����������� ���������� �� ������� �����\n"
                << "SAVE_DB <file>  � ���������� ����� � ����\n"
                << "LOAD_DB <file>  � �������� ����� �� �����\n"
                << "STORE_KEY <halfkey> <text> <ignore> � ����������� ����� ���������\n"
                << "GET_KEY <halfkey> <text>    � �������� SHA-512, ������������ �� �����\n"
                << "PRINT_DATA  � ����� ���� ����������� ������\n"
                << "HELP    � �������� ������\n"
                << "QUIT    � ������� ����������\n";
        }
        // ������ ���������� (�����, �������������� ����� �������)
        else if (cmd == "QUIT") {
            response << "BYE\n";
            send(clientSock, response.str().c_str(), (int)response.str().size(), 0);
            break;
        }
        // �� ������ �������������� �������
        else {
            response << "ERROR: Unknown command\n";
        }
        send(clientSock, response.str().c_str(), (int)response.str().size(), 0);
    }
    // �������� ������
    CLOSESOCKET(clientSock);
}

int main() {
#ifdef _WIN32
    // ������������� Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }
#endif

    // ������������� ������ � �������� �������������
    auto& db = DBServer::getInstance();

    // �������� ���������� ������
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        perror("socket");
        return 1;
    }

    std::cout << "Server started, users loaded from " << DBServer::USERS_CFG << "\n";

    // ������� ������� �������
    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    // �������� ������ � IP � �����
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(serverSock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    // ��������� �������� ����������
    if (listen(serverSock, BACKLOG) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    // ����� �������
    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientSock = accept(serverSock, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        if (clientSock < 0) {
            perror("accept");
            continue; // ��� ������ ���� �������� ���������� �������
        }
        std::thread(handleClient, clientSock).detach();
    }

    // �������� ������
    CLOSESOCKET(serverSock);
#ifdef _WIN32
    WSACleanup();
#endif
    db.saveUsers();
    return 0;
}
