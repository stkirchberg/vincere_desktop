#pragma once
#include <string>
#include <vector>
#include <array>

namespace Vincere {
    class CryptoEngine {
    public:
        // Später ersetzen wir das durch deine eigene C++ Implementierung
        // der Logik aus curve.go
        void generateAndSave(const std::string& privPath, const std::string& pubPath);
        bool loadKeys(const std::string& privPath);
        
        std::string getUsername() { return "stk"; } // Beispiel
        
    private:
        std::array<unsigned char, 32> privKey;
        std::array<unsigned char, 32> pubKey;
    };
}