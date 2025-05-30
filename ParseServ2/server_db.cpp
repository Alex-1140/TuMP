#include "server_db.h"
#include <queue>
#include <fstream>
#include <sstream>
#include <iostream>
#include <QDir>

DBServer& DBServer::getInstance() {
    static DBServer instance;
    return instance;
}

DBServer::DBServer() {
    loadUsers();
}

DBServer::~DBServer() {
    saveUsers();
}

void DBServer::addEdge(Vertex u, Vertex v, Weight w) {
    std::lock_guard<std::mutex> lock(mutex_);
    graph_[u].emplace_back(v, w);
    graph_[v].emplace_back(u, w);
}

std::vector<Weight> DBServer::shortestPath(Vertex src) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto INF = std::numeric_limits<Weight>::infinity();

    Vertex max_v = 0;
    for (auto& kv : graph_) if (kv.first > max_v) max_v = kv.first;

    std::vector<Weight> dist(max_v + 1, INF);
    dist[src] = 0;

    std::priority_queue<std::pair<Weight, Vertex>, std::vector<std::pair<Weight, Vertex>>, std::greater<>> pq;
    pq.emplace(0, src);

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        for (auto& [v, w] : graph_[u]) {
            if (dist[v] > d + w) {
                dist[v] = d + w;
                pq.emplace(dist[v], v);
            }
        }
    }
    return dist;
}

bool DBServer::saveDatabase(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ofstream ofs(filename);
    if (!ofs) return false;

    ofs << "[GRAPH]\n";
    for (auto& [u, edges] : graph_) {
        for (auto& [v, w] : edges) {
            if (u < v)
                ofs << u << ' ' << v << ' ' << w << '\n';
        }
    }

    ofs << "[VIGENERE]\n";
    for (auto& [key_pair, res] : vigenere_halves_) {
        ofs << key_pair.first << '|' << key_pair.second << '|' << res << '\n';
    }
    return true;
}

bool DBServer::loadDatabase(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ifstream ifs(filename);
    if (!ifs) return false;

    graph_.clear();
    vigenere_halves_.clear();

    std::string line;
    enum Section { NONE, GRAPH, VIGENERE } section = NONE;

    while (std::getline(ifs, line)) {
        if (line == "[GRAPH]") {
            section = GRAPH;
        }
        else if (line == "[VIGENERE]") {
            section = VIGENERE;
        }
        else if (!line.empty()) {
            if (section == GRAPH) {
                Vertex u, v; Weight w;
                std::istringstream iss(line);
                iss >> u >> v >> w;
                graph_[u].emplace_back(v, w);
                graph_[v].emplace_back(u, w);
            }
            else if (section == VIGENERE) {
                auto p1 = line.find('|');
                auto p2 = line.find('|', p1 + 1);
                std::string k1 = line.substr(0, p1);
                std::string k2 = line.substr(p1 + 1, p2 - p1 - 1);
                std::string res = line.substr(p2 + 1);
                vigenere_halves_[{k1, k2}] = res;
            }
        }
    }
    return true;
}

void DBServer::storeHalfKey(const std::string& half_key, const std::string& text, const std::string& result) {
    std::lock_guard<std::mutex> lock(mutex_);
    vigenere_halves_[{half_key, text}] = result;
}

bool DBServer::getHalfKey(const std::string& half_key, const std::string& text, std::string& out) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = vigenere_halves_.find({ half_key, text });
    if (it != vigenere_halves_.end()) {
        out = it->second;
        return true;
    }
    return false;
}

void DBServer::printData(std::ostream& os) {
    std::lock_guard<std::mutex> lock(mutex_);
    os << "[GRAPH]\n";
    for (const auto& [u, edges] : graph_) {
        for (const auto& [v, w] : edges) {
            if (u < v) os << u << " -> " << v << " : " << w << '\n';
        }
    }
    os << "[VIGENERE]\n";
    for (const auto& [key_pair, res] : vigenere_halves_) {
        os << "Ключ: " << key_pair.first << ", Текст: " << key_pair.second << ", Результат: " << res << '\n';
    }
    os << "[USERS]\n";
    for (const auto& [login, pass] : users_) {
        os << "Логин: " << login << ", Пароль: " << pass << '\n';
    }
}

std::string DBServer::vigenereEncrypt(const std::string& text, const std::string& key) {
    std::string result = text;
    std::string extended_key = key;
    while (extended_key.length() < text.length()) {
        extended_key += key;
    }
    extended_key = extended_key.substr(0, text.length());

    for (size_t i = 0; i < text.length(); ++i) {
        if (std::isalpha(text[i])) {
            char base = std::isupper(text[i]) ? 'A' : 'a';
            result[i] = (text[i] - base + (extended_key[i] - 'a')) % 26 + base;
        } else if (std::isdigit(text[i])) {
            // Шифрование цифр (диапазон 0-9)
            result[i] = ((text[i] - '0' + (extended_key[i] - 'a') % 10) % 10) + '0';
        }
    }
    std::cerr << "Encrypting text: " << text << " with key: " << key << ", result: " << result << std::endl;
    return result;
}

bool DBServer::registerUser(const std::string& login, const std::string& password) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (users_.find(login) != users_.end()) {
        std::cerr << "User already exists: " << login << std::endl;
        return false;
    }
    std::cerr << "Encrypting password for user: " << login << std::endl;
    std::string encrypted_pass = vigenereEncrypt(password, "secretkey");
    std::cerr << "Encrypted password: " << encrypted_pass << std::endl;
    users_[login] = encrypted_pass;
    std::cerr << "Saving users after registration" << std::endl;
    saveUsers();
    return true;
}

bool DBServer::loginUser(const std::string& login, const std::string& password) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = users_.find(login);
    if (it == users_.end()) {
        std::cerr << "User not found: " << login << std::endl;
        return false;
    }
    std::string encrypted_pass = vigenereEncrypt(password, "secretkey");
    bool success = it->second == encrypted_pass;
    std::cerr << "Login attempt for " << login << ": " << (success ? "Success" : "Failed") << std::endl;
    return success;
}

void DBServer::saveUsers() {
    std::lock_guard<std::mutex> lock(mutex_);
    QDir().mkpath("C:/TuMP/ParseServ2"); // Создать директорию, если не существует
    std::ofstream ofs(USERS_CFG);
    if (!ofs) {
        std::cerr << "Failed to open " << USERS_CFG << " for writing" << std::endl;
        return;
    }
    for (const auto& [login, pass] : users_) {
        ofs << login << '|' << pass << '\n';
    }
    std::cerr << "Users saved to " << USERS_CFG << std::endl;
}

void DBServer::loadUsers() {
    std::lock_guard<std::mutex> lock(mutex_);
    users_.clear();
    std::ifstream ifs(USERS_CFG);
    if (!ifs) {
        std::cerr << "No existing " << USERS_CFG << " found, creating new file" << std::endl;
        QDir().mkpath("C:/TuMP/ParseServ2"); // Создать директорию, если не существует
        std::ofstream ofs(USERS_CFG);
        if (!ofs) {
            std::cerr << "Failed to create " << USERS_CFG << std::endl;
            return;
        }
        ofs.close(); // Создаём пустой файл
        std::cerr << "Created empty " << USERS_CFG << std::endl;
        return;
    }
    std::string line;
    while (std::getline(ifs, line)) {
        auto pos = line.find('|');
        if (pos != std::string::npos) {
            std::string login = line.substr(0, pos);
            std::string pass = line.substr(pos + 1);
            users_[login] = pass;
        }
    }
    std::cerr << "Users loaded from " << USERS_CFG << std::endl;
}
