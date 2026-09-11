#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>

std::string sha256(const std::string& text)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256(reinterpret_cast<const unsigned char*>(text.c_str()), text.size(), hash);

    std::stringstream result;

    for (unsigned char byte : hash) {
        result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }

    return result.str();
}

// int main () {
//     std::string text;
    
//     std::cout << "Enter text : ";
//     std::cin >> text;

//     std::cout << "Hash : " << sha256(text);
// }