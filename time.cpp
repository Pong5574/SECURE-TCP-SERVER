#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <chrono>
#include <ctime>

std::tm* get_local_time () {
    auto now = std::chrono::system_clock::now();

    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::tm* local_time = std::localtime(&time);

    return local_time;
}

std::string get_format_time (const char choice) {
    std::string format;

    switch (choice) {
        case '1':
            format = "%Y-%m-%d";
            break;
        case '2':
            format = "%H-%M-%S";
            break;
        case '3':
            format = "%Y-%m-%d|%H:%M:%S";
            break;
        default:
            return "TIMEERROR";
    }

    std::ostringstream out;
    out << std::put_time(get_local_time(), format.c_str());

    return out.str();
}