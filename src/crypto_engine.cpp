#include "../include/crypto_engine.hpp"
#include <fstream>
#include <sys/stat.h>
#include <cstdint>

namespace Vincere {
    void CryptoEngine::generateAndSave(const std::string& privPath, const std::string& pubPath) {
        system("mkdir -p keys");

        std::ofstream pr(privPath, std::ios::binary);
        std::ofstream pu(pubPath, std::ios::binary);
        
        uint8_t dummy = 0x42; //just a test value
        for(int i=0; i<32; i++) {
            pr.put(dummy);
            pu.put(dummy);
        }
    }

    bool CryptoEngine::loadKeys(const std::string& privPath) {
        std::ifstream f(privPath, std::ios::binary);
        return f.good();
    }

    std::string CryptoEngine::getUsername() const {
        return "Sonja (Arch)";
    }
}