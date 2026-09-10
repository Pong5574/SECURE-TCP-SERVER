#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <vector>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "time.h"
#include "db_server.h"

#pragma comment(lib, "ws2_32.lib")

struct ACC_DATA {
    SOCKET sock;
    std::string ip;
    unsigned int port;
    std::string host_name;
};

struct packet {
    uint8_t type;
    uint32_t size;
};

std::vector<ACC_DATA> acc_list;

int main () {
    WSADATA win_sock_api;

    const int link = WSAStartup(MAKEWORD(2, 2), &win_sock_api);

    if (link != 0) {
        std::cerr << '[' << get_format_time('3') << ']' << " Error code (" << link << ") WSAStartup failed\n";

        return 1;
    }

    SOCKET srv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (srv_sock == INVALID_SOCKET) {
        std::cerr << '[' << get_format_time('3') << ']' << " Error code (" << WSAGetLastError() << ") Socket creation failed\n";

        WSACleanup();

        return 1;
    }

    sockaddr_in srv_addr{};

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(8080);
    srv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(srv_sock, reinterpret_cast<sockaddr*>(&srv_addr), sizeof(srv_addr)) == SOCKET_ERROR) {
        std::cerr << '[' << get_format_time('3') << ']' << " Error code (" << WSAGetLastError() << ") Binding failed\n";

        closesocket(srv_sock);
        WSACleanup();

        return 1;
    }

    if (listen(srv_sock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << '[' << get_format_time('3') << ']' << " Error code (" << WSAGetLastError() << ") Listening failed\n";

        closesocket(srv_sock);
        WSACleanup();

        return 1;
    }

    std::cout << '[' << get_format_time('3') << ']' << " Server start on port " << ntohs(srv_addr.sin_port) << '\n';
    std::cout << '[' << get_format_time('3') << ']' << " Waiting for client....\n";

    if (!create_table()) {
        return 1;
    }

    while (true) {
        sockaddr_in clt_addr{};

        int clt_addr_size = sizeof(clt_addr);

        SOCKET clt_sock = accept(srv_sock, reinterpret_cast<sockaddr*>(&clt_addr), &clt_addr_size);

        if (clt_sock == INVALID_SOCKET) {
            continue;
        }

        bool add;
        
        char clt_ip[INET_ADDRSTRLEN];

        const unsigned int clt_port = ntohs(clt_addr.sin_port);

        inet_ntop(AF_INET, &clt_addr.sin_addr, clt_ip, INET_ADDRSTRLEN);

        char buffer[1024];

        const int received = recv(clt_sock, buffer, sizeof(buffer) - 1, 0);

        std::string sent_clt;

        if (received > 0) {
            buffer[received] = '\0';
            
            std::string temp = buffer;

            size_t p1 = temp.find('/');
            size_t p2 = temp.find('/', p1 + 1);

            if (p1 == std::string::npos || p2 == std::string::npos) {
                std::cerr << '[' << get_format_time('3') << ']' << " Invalid packet format\n";

                closesocket(clt_sock);

                continue;
            }

            // type/user|pass/size

            const int int_type = std::stoi(temp.substr(0, p1));
            std::string data = temp.substr(p1 + 1, p2 - p1 - 1);
            int size = std::stoi(temp.substr(p2 + 1));

            std::string type;

            switch (int_type) {
                case 0:
                    {
                        type = "REGISTER";

                        size_t pos = data.find('|');

                        if (pos == std::string::npos) {
                            std::cerr << '[' << get_format_time('3') << ']' << " Invalid username and password format\n";

                            closesocket(clt_sock);

                            continue;
                        }

                        std::string user = data.substr(0, pos);
                        std::string pass = data.substr(pos + 1);

                        if (register_check(user, pass)) {
                            add = true;

                            sent_clt = "Register successfully";
                        } else {
                            std::cout << '[' << get_format_time('3') << "] " << clt_ip << ':' << clt_port << " Cannot create account\n";

                            sent_clt = "Register failed";

                            closesocket(clt_sock);

                            add = false;
                        }
                    }

                    break;
                case 1:
                    {
                        type = "LOGIN";

                        size_t pos = data.find('|');

                        if (pos == std::string::npos) {
                            std::cerr << '[' << get_format_time('3') << ']' << " Invalid username and password format\n";

                            closesocket(clt_sock);

                            continue;
                        }

                        std::string user = data.substr(0, pos);
                        std::string pass = data.substr(pos + 1);

                        if (login_check(user, pass)) {
                            add = true;

                            sent_clt = "Login successfully";
                        } else {
                            std::cout << '[' << get_format_time('3') << ']' << " Invalid password or username\n";

                            sent_clt = "Login failed";

                            closesocket(clt_sock);

                            add = false;
                        }
                    }

                    break;
                default:
                    std::cerr << '[' << get_format_time('3') << "] " << clt_ip << ':' << clt_port << " Attemping to login\n";
                    
                    add = false;

                    break;
            }

        } else if (received == 0) {
            std::cout << '[' << get_format_time('3') << "] " << clt_ip << ':' << clt_port << " Attemping to login\n";

            closesocket(clt_sock);

            add = false;

            continue;
        } else {
            std::cerr << '[' << get_format_time('3') << ']' << " Error code (" << WSAGetLastError() << ") Recv failed\n";

            closesocket(clt_sock);

            add = false;
            
            continue;
        }

        if (add) {
            send(clt_sock, sent_clt.c_str(), static_cast<int>(sent_clt.size()), 0);

            ACC_DATA curr_acc;

            curr_acc.sock = clt_sock;
            curr_acc.ip = clt_ip;
            curr_acc.port = clt_port;

            acc_list.push_back(curr_acc);

            std::cout << '[' << get_format_time('3') << "] " << clt_ip << ':' << clt_port << " | CONNECTED\n";
        }
    }

    closesocket(srv_sock);
    WSACleanup();

    return 0;
}