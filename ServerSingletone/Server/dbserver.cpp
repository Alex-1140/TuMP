#include "dbserver.h"
#include <fstream>
#include <queue>

// Реализация зависит от платформы: Win — CryptoAPI, Unix — OpenSSL
#ifdef _WIN32
#  include <windows.h>
#  include <wincrypt.h>
#else
#  include <openssl/sha.h>
#endif

#include <sstream>
#include <iomanip>
#include <algorithm>

// windows.h подтянул min и max, нужно их убрать
#undef max
#undef min

// вспомогательная функция вычисления sha512
static std::string sha512(const std::string& data) {

#ifdef _WIN32
    // Win CryptoAPI: создание хэш-объекта и считывание SHA-512
    HCRYPTPROV hProv = NULL;
    HCRYPTHASH hHash = NULL;
    BYTE hash[64];
    DWORD hashLen = sizeof(hash);

    // Получение контекста
    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
        return {};

    // Создание хеш-объекта
    if (!CryptCreateHash(hProv, CALG_SHA_512, 0, 0, &hHash)) {
        CryptReleaseContext(hProv, 0);
        return {};
    }

    // Хеширование данных
    CryptHashData(hHash,
        reinterpret_cast<const BYTE*>(data.data()),
        static_cast<DWORD>(data.size()),
        0);

    // Результат
    CryptGetHashParam(hHash, HP_HASHVAL, hash, &hashLen, 0);

    // Освобождение ресурсов
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);

    // Перевод в hex-строку
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (DWORD i = 0; i < hashLen; ++i)
        oss << std::setw(2) << static_cast<int>(hash[i]);
    return oss.str();

#else
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512(reinterpret_cast<const unsigned char*>(data.data()),
        data.size(),
        hash);

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (int i = 0; i < SHA512_DIGEST_LENGTH; ++i)
        oss << std::setw(2) << static_cast<int>(hash[i]);
    return oss.str();
#endif
}


// Получение единственного экземпляра класса (синглтон)
DBServer& DBServer::getInstance() {
    static DBServer instance;
    return instance;
}

// Загрузка пользователей при создании экземпляра класса
DBServer::DBServer() {
    loadUsers();
}

// Сохранение пользователей при уничтожении экземпляра класса
DBServer::~DBServer() { 
    saveUsers();
}

// Регистрация: возвращает false, если такой пользователь уже существует
// true — регистрация успешна
bool DBServer::registerUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (users_.count(username))
        return false;
    users_[username] = sha512(password); // Шифруется пароль
    saveUsers();  // запись данных в файл
    return true;
}

// Шифрование текста методом Виженера с использованием заданного ключа
static std::string vigenereEncrypt(const std::string& key, const std::string& text) {
    std::string out;
    out.reserve(text.size());
    int k = key.size();
    for (size_t i = 0, j = 0; i < text.size(); ++i) {
        char c = text[i];
        unsigned shift = static_cast<unsigned char>(key[j % k]) % 26;
        if (std::isupper((unsigned char)c)) {
            out.push_back(char((c - 'A' + shift) % 26 + 'A')); ++j;
        }
        else if (std::islower((unsigned char)c)) {
            out.push_back(char((c - 'a' + shift) % 26 + 'a')); ++j;
        }
        else {
            out.push_back(c);
        }
    }
    return out;
}

// Проверка логина и пароля, возвращает true при успехе
std::string DBServer::vigenereEncrypt(const std::string& key, const std::string& text) {
    std::string out;
    out.reserve(text.size());
    size_t keyLen = key.size();
    for (size_t i = 0, j = 0; i < text.size(); ++i) {
        char c = text[i];
        unsigned shift = static_cast<unsigned char>(key[j % keyLen]) % 26;
        if (std::isupper(static_cast<unsigned char>(c))) {
            out.push_back(char((c - 'A' + shift) % 26 + 'A'));
            ++j;
        }
        else if (std::islower(static_cast<unsigned char>(c))) {
            out.push_back(char((c - 'a' + shift) % 26 + 'a'));
            ++j;
        }
        else {
            out.push_back(c);
        }
    }
    return out;
}

// Логин: проверка наличия и совпадения пароля
// true — данные совпадают, пользователь проходит
bool DBServer::loginUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = users_.find(username);
    return it != users_.end() && it->second == sha512(password);
}

// Загрузка базу зарегистрированных пользователей из
// конфигурационного файла. Создается новый файл, если
// таковой отсутствует
void DBServer::loadUsers() {
    std::ifstream fin(USERS_CFG);
    if (!fin.is_open()) {
        // Файл не существует — создается новый
        std::ofstream fout(USERS_CFG);
        return;
    }
    std::string user, pass;
    while (fin >> user >> pass) {
        users_[user] = pass;
    }
}
// Сохранение базы пользователей в конфигурационный файл
void DBServer::saveUsers() {
    std::ofstream fout(USERS_CFG, std::ios::trunc);
    for (const auto& kv : users_) {
        fout << kv.first << ' ' << kv.second << '\n';
    }
}
// Добавление ребра в граф с весом
void DBServer::addEdge(Vertex u, Vertex v, Weight w) {
    std::lock_guard<std::mutex> lock(mutex_);
    graph_[u].emplace_back(v, w);
    graph_[v].emplace_back(u, w);
}
// Вычисление кратчайшего пути от вершины до остальных
// (Алгоритм Дейкстры)
std::vector<Weight> DBServer::shortestPath(Vertex src) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::map<Vertex, Weight> dist;
    for (auto& [v, _] : graph_) dist[v] = std::numeric_limits<Weight>::infinity();
    dist[src] = 0;
    using P = std::pair<Weight, Vertex>;
    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.emplace(0, src);
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        for (auto& [v, w] : graph_[u]) {
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                pq.emplace(dist[v], v);
            }
        }
    }
    Vertex maxV = 0;
    for (auto& [v, _] : dist) maxV = std::max(maxV, v);
    std::vector<Weight> result(maxV + 1, std::numeric_limits<Weight>::infinity());
    for (auto& [v, d] : dist) result[v] = d;
    return result;
}

// Сохранение графа и данных Виженера в файл
bool DBServer::saveDatabase(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) return false;
    size_t sz = graph_.size();
    ofs.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
    for (auto& [u, vec] : graph_) {
        ofs.write(reinterpret_cast<const char*>(&u), sizeof(u));
        size_t edges = vec.size();
        ofs.write(reinterpret_cast<const char*>(&edges), sizeof(edges));
        for (auto& [v, w] : vec) {
            ofs.write(reinterpret_cast<const char*>(&v), sizeof(v));
            ofs.write(reinterpret_cast<const char*>(&w), sizeof(w));
        }
    }
    size_t m = vigenere_halves_.size();
    ofs.write(reinterpret_cast<const char*>(&m), sizeof(m));
    for (auto& [key, val] : vigenere_halves_) {
        auto& [half, text] = key;
        size_t len1 = half.size(), len2 = text.size(), len3 = val.size();
        ofs.write(reinterpret_cast<const char*>(&len1), sizeof(len1));
        ofs.write(half.data(), len1);
        ofs.write(reinterpret_cast<const char*>(&len2), sizeof(len2));
        ofs.write(text.data(), len2);
        ofs.write(reinterpret_cast<const char*>(&len3), sizeof(len3));
        ofs.write(val.data(), len3);
    }
    return true;
}

// Загрузка графа и данных Виженера из файла
bool DBServer::loadDatabase(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) return false;
    graph_.clear();
    size_t sz;
    ifs.read(reinterpret_cast<char*>(&sz), sizeof(sz));
    for (size_t i = 0; i < sz; ++i) {
        Vertex u;
        ifs.read(reinterpret_cast<char*>(&u), sizeof(u));
        size_t edges;
        ifs.read(reinterpret_cast<char*>(&edges), sizeof(edges));
        for (size_t j = 0; j < edges; ++j) {
            Vertex v; Weight w;
            ifs.read(reinterpret_cast<char*>(&v), sizeof(v));
            ifs.read(reinterpret_cast<char*>(&w), sizeof(w));
            graph_[u].emplace_back(v, w);
        }
    }
    size_t m;
    ifs.read(reinterpret_cast<char*>(&m), sizeof(m));
    vigenere_halves_.clear();
    for (size_t i = 0; i < m; ++i) {
        size_t len1, len2, len3;
        ifs.read(reinterpret_cast<char*>(&len1), sizeof(len1));
        std::string half(len1, '\0'); ifs.read(&half[0], len1);
        ifs.read(reinterpret_cast<char*>(&len2), sizeof(len2));
        std::string text(len2, '\0'); ifs.read(&text[0], len2);
        ifs.read(reinterpret_cast<char*>(&len3), sizeof(len3));
        std::string val(len3, '\0'); ifs.read(&val[0], len3);
        vigenere_halves_[{half, text}] = val;
    }
    return true;
}

// Сохранение половины ключа и текста, включая зашифрованный результат через Виженера
// и SHA-512
void DBServer::storeHalfKey(const std::string& half_key,
    const std::string& text,
    const std::string&) {
    std::string fullKey = sha512(half_key); // Генерация полного ключа из половины через SHA-512
    std::string cipher = vigenereEncrypt(fullKey, text); // Шифрование текста
    std::string cipherHash = sha512(cipher); // Вычисление хэш зашифрованного текста
    std::lock_guard<std::mutex> lock(mutex_);
    vigenere_halves_[{half_key, text}] = cipherHash;
//    vigenere_halves_[{half_key, text}] = result;
}

// Получение сохраненного результата шифрования для половины ключа и текста
bool DBServer::getHalfKey(const std::string& half_key,
    const std::string& text,
    std::string& out) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = vigenere_halves_.find({ half_key, text });
    if (it == vigenere_halves_.end()) return false;
    out = it->second;
    return true;
}

// Вывод данных графа и сохраненных половины ключей в указанном потоке
void DBServer::printData(std::ostream& os) {
    std::lock_guard<std::mutex> lock(mutex_);
    os << "Graph data:\n";
    for (auto& [u, vec] : graph_) {
        for (auto& [v, w] : vec) {
            os << u << " -> " << v << " (" << w << ")\n";
        }
    }
    os << "Vigenere halves:\n";
    for (auto& [key, val] : vigenere_halves_) {
        os << "[" << key.first << "," << key.second << "] = " << val << "\n";
    }
}