#include <iostream>

#include <string>

#include "sqlite3.h"

int open_db (sqlite3** db) {
    const int result = sqlite3_open("server.db", db);

    if (result != SQLITE_OK) {
        return 1;
    }

    return 0;
}

int create_table (sqlite3* db) {
    const char* sql_cmd = "CREATE TABLE IF NOT EXISTS User (Username TEXT UNIQUE NOT NULL, Password TEXT NOT NULL);";

    int result = sqlite3_exec(db, sql_cmd, nullptr, nullptr, 0);

    if (result != SQLITE_OK) {
        return 1;
    }

    return 0;
}

int login (sqlite3* db, const std::string& username, const std::string& password) {
    sqlite3_stmt* stmt;

    const char* sql_cmd = "SELECT Password FROM User WHERE Username = ?";

    sqlite3_prepare_v2(db, sql_cmd, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

        if (hash == nullptr && password != hash) {
            sqlite3_finalize(stmt);

            return 1;
        }
    }

    sqlite3_finalize(stmt);

    return 0;
}

// int main () {
//     sqlite3* db;

//     if (open_db(&db) == SQLITE_OK) {
//         std::cerr << "Open database failed\n";

//         return 1;
//     }

//     if (create_table(db) == SQLITE_OK) {
//         std::cerr << "Creating table failed\n";

//         sqlite3_close(db);

//         return 1;
//     }

//     const char* sql_cmd = "INSERT INTO User (Username, Password) VALUES ('admin', '1234')";

//     int result = sqlite3_exec(db, sql_cmd, nullptr, nullptr, 0);    

//     if (result == SQLITE_OK) {
//         std::cout << "Insert success\n";
//     } else {
//         std::cerr << "Insert failed.\n";

//         sqlite3_close(db);

//         return 1;
//     }

//     sqlite3_close(db);

//     return 0;
// }