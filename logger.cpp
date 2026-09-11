#include <string>
#include <fstream>

void logger (const std::string& file_name, const std::string& msg) {
    std::ofstream file(file_name, std::ios::app);

    file << msg;

    file.close();
}