#pragma once

#include <string>

#include "sqlite3.h"

int open_db (sqlite3** db);

int create_table (sqlite3* db);

int login (sqlite3* db, const std::string& username, const std::string& password);