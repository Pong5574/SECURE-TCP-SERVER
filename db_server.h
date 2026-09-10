#pragma once

#include <string>

bool open_db ();

bool execute (const std::string& sql);

bool check_user (const std::string& user);

bool check_pass (const std::string& pass);

bool register_check (const std::string& user, const std::string& pass);

bool login_check (const std::string& user, const std::string& pass);

bool create_table ();