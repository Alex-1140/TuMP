#include "Vigenere.h"
#include <sstream>
#include <fstream>

void Vigenere::storeHalfKey(const std::string& half_key, const std::string& text, const std::string& result) {
    std::lock_guard<std::mutex> lock(mutex_);
    vigenere_halves_[{half_key, text}] = result;
}

bool Vigenere::getHalfKey(const std::string& half_key, const std::string& text, std::string& out) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = vigenere_halves_.find({ half_key, text });
    if (it != vigenere_halves_.end()) {
        out = it->second;
        return true;
    }
    return false;
}

void Vigenere::saveVigenere(std::ostream& ofs) const {
    std::lock_guard<std::mutex> lock(mutex_);
    ofs << "[VIGENERE]\n";
    for (auto& [key_pair, res] : vigenere_halves_) {
        ofs << key_pair.first << '|' << key_pair.second << '|' << res << '\n';
    }
}

void Vigenere::loadVigenere(std::ifstream& ifs) {
    std::lock_guard<std::mutex> lock(mutex_);
    vigenere_halves_.clear();
    std::string line;
    while (std::getline(ifs, line)) {
        if (!line.empty()) {
            auto p1 = line.find('|');
            auto p2 = line.find('|', p1 + 1);
            std::string k1 = line.substr(0, p1);
            std::string k2 = line.substr(p1 + 1, p2 - p1 - 1);
            std::string res = line.substr(p2 + 1);
            vigenere_halves_[{k1, k2}] = res;
        }
    }
}
