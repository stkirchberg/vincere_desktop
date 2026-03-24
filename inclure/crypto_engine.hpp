#ifndef CRYPTO_ENGINE_HPP
#define CRYPTO_ENGINE_HPP

#include <string>
#include <vector>
#include <array>

namespace Vincere {
    class CryptoEngine {
    public:
        void generateAndSave(const std::string& privPath, const std::string& pubPath);
        bool loadKeys(const std::string& privPath);
        std::string getUsername() const;
        
    private:
        std::array<uint8_t, 32> privKey;
        std::array<uint8_t, 32> pubKey;
    };
}

#endif