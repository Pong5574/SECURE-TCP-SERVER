#include <iostream>
#include <string>

#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int sent_data (const SOCKET sock, const char* data, const int data_size) {
    int total_send = 0;

    while (total_send < data_size) {
        const int sent = send(sock, data + total_send, data_size - total_send, 0);

        if (sent == SOCKET_ERROR) {
            return -1;
        } else if (sent == 0) {
            return 0;
        }

        total_send += sent;
    }

    return total_send;
}

int recv_data (const SOCKET sock, char* data, const int data_size) {
    int total_recv = 0;

    while (total_recv < data_size) {
        const int rece = recv(sock, data + total_recv, data_size - total_recv, 0);

        if (rece == SOCKET_ERROR) {
            return -1;
        } else if (rece == 0) {
            return 0;
        }

        total_recv += rece;
    }

    return total_recv;
}