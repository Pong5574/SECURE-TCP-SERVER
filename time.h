#pragma once

#include <string>
#include <ctime>

std::tm* get_local_time ();

std::string get_format_time (const char choice);