#pragma once

#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int sent_data (const SOCKET sock, const char* data, const int data_size);

int recv_data (const SOCKET sock, char* data, const int data_size);