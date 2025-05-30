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
    static constexpr const char* USERS_CFG = "C:/TuMP/ParseServ2/users.cfg";

    static DBServer& getInstance();
    DBServer(const DBServer&) = delete;
    DBServer& operator=(const DBServer&) = delete;

    void addEdge(Vertex u, Vertex v, Weight w);
    std::vector<Weight> shortestPath(Vertex src);
    static std::string vigenereEncrypt(const std::string&, const std::string&);
    bool saveDatabase(const std::string& filename);
    bool loadDatabase(const std::string& filename);
    void storeHalfKey(const std::string& half_key, const std::string& text, const std::string& result);
    bool getHalfKey(const std::string& half_key, const std::string& text, std::string& out);
    void printData(std::ostream& os = std::cout);
    bool registerUser(const std::string&, const std::string&);
    bool loginUser(const std::string&, const std::string&);
    void loadUsers();
    void saveUsers();

private:
    DBServer();
    ~DBServer();
    AdjList graph_;
    std::map<std::pair<std::string, std::string>, std::string> vigenere_halves_;
    std::map<std::string, std::string> users_;
    std::mutex mutex_;
};
