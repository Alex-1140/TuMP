#ifndef VIGENERE_H
#define VIGENERE_H

#include <map>
#include <string>
#include <mutex>
#include <ostream>
#include <fstream>

class Vigenere {
public:
    Vigenere() = default;
    ~Vigenere() = default;

    void storeHalfKey(const std::string& half_key, const std::string& text, const std::string& result);
    bool getHalfKey(const std::string& half_key, const std::string& text, std::string& out);
    void saveVigenere(std::ostream& ofs) const;
    void loadVigenere(std::ifstream& ifs);

private:
    std::map<std::pair<std::string, std::string>, std::string> vigenere_halves_;
    mutable std::mutex mutex_; // Made mutable to allow locking in const methods
};

#endif // VIGENERE_H
