#include <iostream>
#include <string>
#include <cctype>

#include "sqlite3.h"
#include "time.h"

sqlite3* db = nullptr;

bool open_db () {
    const int open_db = sqlite3_open("server.db", &db);

    if (open_db != SQLITE_OK) {
        std::cerr << '[' << get_format_time('3') << ']' << " Error (" << sqlite3_errmsg(db) << ")Database open failed\n";

        return false;
    }

    std::cout << '[' << get_format_time('3') << ']' << " Database opened successfully\n";

    return true;
}

bool execute (const std::string& sql) {
    char* err = nullptr;

    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err);

    if (result != SQLITE_OK) {
        std::cerr << '[' << get_format_time('3') << ']' << " Error (" << err << ") Execute failed\n";

        sqlite3_free(err);

        return false;
    }

    return true;
}

bool check_user (const std::string& user) {
    if (user.empty()) {
        return false;
    }

    if (user.length() < 3 || user.length() > 32) {
        return false;
    }

    for (char c : user) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '-') {
            return false;
        }
    }

    return true;
}

bool check_pass (const std::string& pass) {
    if (pass.empty()) {
        return false;
    }

    if (pass.length() < 12 || pass.length() > 128) {
        return false;
    }

    for (char c : pass) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '-') {
            return false;
        }
    }

    return true;
}

bool register_check (const std::string& user, const std::string& pass) {
    if (!open_db()) {
        return 1;
    }

    sqlite3_stmt* stmt;

    const std::string sql = "INSERT INTO Users (Username, Password) VALUES (?, ?)";

    // if (!check_user(user) || !check_pass(pass)) {
    //     std::cerr << '[' << get_format_time('3') << ']' << " Error (" << ") Execute failed\n";

    //     return false;
        
    // }

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr)) {
        sqlite3_close(db);

        return false;
    }

    if (sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_STATIC) != SQLITE_OK || sqlite3_bind_text(stmt, 2, pass.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        return false;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return true;
}

bool login_check (const std::string& user, const std::string& pass) {
    if (!open_db()) {   
        return false;
    }

    // if (!check_user(user) || !check_pass(pass)) {
    //     return false;
    // }

    sqlite3_stmt* stmt;

    const std::string sql = "SELECT Password FROM Users WHERE Username = ?";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);

        return false;
    }

    if (sqlite3_bind_text(stmt, 1, user.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        return false;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        return false;
    }

    const char* hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

    if (pass != hash) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return true;
}

bool create_table () {
    if (!open_db()) {   
        return false;
    }


    if (!execute("CREATE TABLE IF NOT EXISTS Users (ID INTEGER PRIMARY KEY AUTOINCREMENT, Username TEXT NOT NULL UNIQUE, Password TEXT NOT NULL);")) {
        sqlite3_close(db);

        return false;
    }

    sqlite3_close(db);

    return true;
}

    // const char* sql = "CREATE TABLE IF NOT EXISTS Users (ID INTEGER PRIMARY KEY AUTOINCREMENT, Username TEXT NOT NULL UNIQUE, Password TEXT NOT NULL);";

    // if (!open_db()) {   
    //     return 1;
    // }


    // if (!execute("CREATE TABLE IF NOT EXISTS Users (ID INTEGER PRIMARY KEY AUTOINCREMENT, Username TEXT NOT NULL UNIQUE, Password TEXT NOT NULL);")) {
    //     sqlite3_close(db);

    //     return 1;
    // }

    // sqlite3_close(db);