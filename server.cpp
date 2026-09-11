#define SERVER_PORT_TEST 8080

#define SYN_TO_SERVER "de7d1b721a1e0632b7cf04edf5032c8ecffa9f9a08492152b926f1a5a7e765d7" // i need connect
#define SYN_ACK_TO_CLIENT "a7722b765d15608d5d24a319140e6665cb361a0758285125722a2d5aece38808" // well come to server
#define ACK_TO_SERVER "ab6db599234d2636659cba1aa191bd014c3867d5cfade98ff694785c20c28fc6" // am connect success
#define LOGIN_RESPONSE "428821350e9691491f616b754cd8315fb86d797ab35d843479e732ef90665324" // login success

#include <iostream>
#include <string>
#include <vector>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "database.h"
#include "sqlite3.h"
#include "packet.h"
#include "logger.h"
#include "time.h"

#pragma comment(lib, "ws2_32.lib")

struct CLT_DATA {
    SOCKET sock;
    std::string ip;
    int port;
    std::string host_name;
};

std::vector<CLT_DATA> clt_list;

const std::string file_log = "server.log";

int main () {
    WSADATA win_sock_api;

    const int start_up = WSAStartup(MAKEWORD(2, 2), &win_sock_api);

    if (start_up != 0) {
        std::cerr << get_format_time('3') << " Error code (" << start_up << ") Start winsock api failed\n";

        logger(file_log ,get_format_time('3') + " Error code (" + std::to_string(start_up) + ") Start winsock api failed\n");

        return 1;
    }

    SOCKET srv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (srv_sock == INVALID_SOCKET) {
        std::cerr << get_format_time('3') << " Error code (" << WSAGetLastError() << ") Socket creation failed\n";

        logger(file_log ,get_format_time('3') + " Error code (" + std::to_string(WSAGetLastError()) + ") Socket creation failed\n");

        WSACleanup();

        return 1;
    }

    sockaddr_in srv_addr{};

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(SERVER_PORT_TEST);
    srv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(srv_sock, reinterpret_cast<sockaddr*>(&srv_addr), sizeof(srv_addr)) == SOCKET_ERROR) {
        std::cerr << get_format_time('3') << " Error code (" << WSAGetLastError << ") binding failed\n";

        logger(file_log ,get_format_time('3') + "Error code (" + std::to_string(WSAGetLastError()) + ") binding failed\n");

        closesocket(srv_sock);
        WSACleanup();

        return 1;
    }

    if (listen(srv_sock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << get_format_time('3') << " Error code (" << WSAGetLastError() << ") Listening failed\n";

        logger(file_log ,get_format_time('3') + "Error code (" + std::to_string(WSAGetLastError()) + ") Listening failed\n");

        closesocket(srv_sock);
        WSACleanup();

        return 1;
    }

    std::cout << get_format_time('3') << " Server start on port " << SERVER_PORT_TEST << '\n';

    logger(file_log ,get_format_time('3') + " Server start on port " + std::to_string(SERVER_PORT_TEST) + '\n');

    while (true) {
        sockaddr_in clt_addr{};
        int clt_addr_size = sizeof(clt_addr);

        SOCKET clt_sock = accept(srv_sock, reinterpret_cast<sockaddr*>(&clt_addr), &clt_addr_size);

        if (clt_sock == INVALID_SOCKET) {
            continue;
        }

        char clt_ip[INET_ADDRSTRLEN];
        const int clt_port = ntohs(clt_addr.sin_port);

        inet_ntop(AF_INET, &clt_addr.sin_addr, clt_ip, INET_ADDRSTRLEN);

        {
            char buffer[71]{};
            const int rece = recv_data(clt_sock, buffer, sizeof(buffer));

            if (rece == SOCKET_ERROR) {
                std::cerr << get_format_time('3') << " Recv SYN from client failed\n";

                logger(file_log ,get_format_time('3') + " Recv SYN from client failed\n");

                closesocket(clt_sock);

                continue;
            } else if (rece == 0) {
                std::cerr << get_format_time('3') << " Client offline\n";

                logger(file_log ,get_format_time('3') + " Client offline\n");

                closesocket(clt_sock);

                continue;
            } else {
                std::string temp_buffer = buffer;

                size_t p1 = temp_buffer.find('/');
                size_t p2 = temp_buffer.find('/', p1 + 1);

                if (p1 == std::string::npos || p2 == std::string::npos) {
                    std::cerr << get_format_time('3') << " Invalid packet format\n";

                    logger(file_log ,get_format_time('3') + " Invalid packet format\n");

                    closesocket(clt_sock);

                    continue;
                }

                // type/data/size

                const std::string type = temp_buffer.substr(0, p1);
                std::string data = temp_buffer.substr(p1 + 1, p2 - p1 - 1);
                int size = std::stoi(temp_buffer.substr(p2 + 1));

                if (type != "SYN" || data != SYN_TO_SERVER) {
                    std::cerr << get_format_time('3') << " Error packet SYN\n";

                    logger(file_log ,get_format_time('3') + " Error packet SYN\n");

                    closesocket(clt_sock);

                    continue;
                }
            }
        }

        {
            const std::string packet = "SYN-ACK/" + std::string(SYN_ACK_TO_CLIENT) + "/" + std::to_string(std::string(SYN_ACK_TO_CLIENT).size());
            const int sent = sent_data(clt_sock, packet.c_str(), packet.size());

            // std::cout << "Send packet size: " << packet.size() << '\n';

            if (sent == SOCKET_ERROR) {
                std::cerr << get_format_time('3') << " Send SYN-ACK to client failed\n";

                logger(file_log ,get_format_time('3') + " Send SYN-ACK to client failed\n");

                closesocket(clt_sock);

                continue;
            }else if (sent == 0) {
                std::cerr << get_format_time('3') << " Client offline\n";

                logger(file_log ,get_format_time('3') + " Client offline\n");

                closesocket(clt_sock);

                continue;
            }
        }

        {
            char buffer[71]{};
            const int rece = recv_data(clt_sock, buffer, sizeof(buffer));

            if (rece == SOCKET_ERROR) {
                std::cerr << get_format_time('3') << " Recv ACK from client failed\n";

                logger(file_log ,get_format_time('3') + " Recv ACK from client failed\n");

                closesocket(clt_sock);

                continue;
            } else if (rece == 0) {
                std::cerr << get_format_time('3') << " Client offline\n";

                logger(file_log ,get_format_time('3') + " Client offline\n");

                closesocket(clt_sock);

                continue;
            } else {
                std::string temp_buffer = buffer;

                size_t p1 = temp_buffer.find('/');
                size_t p2 = temp_buffer.find('/', p1 + 1);

                if (p1 == std::string::npos || p2 == std::string::npos) {
                    std::cerr << get_format_time('3') << " Invalid packet format\n";

                    logger(file_log ,get_format_time('3') + " Invalid packet format\n");

                    closesocket(clt_sock);

                    continue;
                }

                // type/data/size

                const std::string type = temp_buffer.substr(0, p1);
                std::string data = temp_buffer.substr(p1 + 1, p2 - p1 - 1);
                int size = std::stoi(temp_buffer.substr(p2 + 1));

                if (type != "ACK" || data != ACK_TO_SERVER) {
                    std::cerr << get_format_time('3') << " Error packet ACK\n";

                    logger(file_log ,get_format_time('3') + " Error packet ACK\n");

                    closesocket(clt_sock);

                    continue;
                }
            }
        }

        std::cout << get_format_time('3') << ' ' << clt_ip << ':' << clt_port << " | Connected\n";

        logger(file_log ,get_format_time('3') + ' ' + clt_ip + std::to_string(clt_port) + " | Connected\n");

        {
            char buffer[1024]{};
            const int rece = recv(clt_sock, buffer, sizeof(buffer), 0);

            if (rece == SOCKET_ERROR) {
                std::cerr << get_format_time('3') << " Recv ACK from client failed\n";

                logger(file_log ,get_format_time('3') + " Recv ACK from client failed\n");

                closesocket(clt_sock);

                continue;
            } else if (rece == 0) {
                std::cerr << get_format_time('3') << " Client offline\n";

                logger(file_log ,get_format_time('3') + " Client offline\n");

                closesocket(clt_sock);

                continue;
            } else {
                std::string temp_buffer = buffer;

                size_t p1 = temp_buffer.find('/');
                size_t p2 = temp_buffer.find('/', p1 + 1);

                if (p1 == std::string::npos || p2 == std::string::npos) {
                    std::cerr << get_format_time('3') << " Invalid packet format\n";

                    logger(file_log ,get_format_time('3') + " Invalid packet format\n");

                    closesocket(clt_sock);

                    continue;
                }

                // type/data/size

                const std::string type = temp_buffer.substr(0, p1);
                std::string data = temp_buffer.substr(p1 + 1, p2 - p1 - 1);
                int size = std::stoi(temp_buffer.substr(p2 + 1));

                size_t position = data.find('|');

                if (position == std::string::npos) {
                    std::cerr << get_format_time('3') << " Invalid data format\n";

                    logger(file_log ,get_format_time('3') + " Invalid data format\n");

                    closesocket(clt_sock);

                    continue;
                }

                std::string username = data.substr(0, position);
                std::string password = data.substr(position + 1);

                sqlite3* db;

                if (open_db(&db) != SQLITE_OK) {
                    std::cerr << get_format_time('3') << "Open database failed\n";

                    logger(file_log, get_format_time('3') + " Open database failed\n");

                    closesocket(clt_sock);

                    continue;
                }

                if (create_table(db) != SQLITE_OK) {
                    std::cerr << get_format_time('3') << " Creation table failed\n";

                    logger(file_log, get_format_time('3') + " Creation table failed\n");

                    closesocket(clt_sock);

                    continue;
                }

                if (login(db, username, password) != SQLITE_OK || type != "LOGIN-REQUEST") {
                    std::cerr << get_format_time('3') << " Invalid username or password\n";

                    logger(file_log, get_format_time('3') + " Invalid username or password\n");

                    closesocket(clt_sock);

                    continue;
                }
            }
        }

        {
            const std::string packet = "LOGIN-RESPONSE/" + std::string(LOGIN_RESPONSE) + "/" + std::to_string(std::string(LOGIN_RESPONSE).size());
            const int sent = sent_data(clt_sock, packet.c_str(), packet.size());

            // std::cout << "Send packet size: " << packet.size() << '\n';

            if (sent == SOCKET_ERROR) {
                std::cerr << get_format_time('3') << " Send login response failed\n";

                logger(file_log ,get_format_time('3') + " Send login response failed\n");

                closesocket(clt_sock);

                continue;
            }else if (sent == 0) {
                std::cerr << get_format_time('3') << " Client offline\n";

                logger(file_log ,get_format_time('3') + " Client offline\n");

                closesocket(clt_sock);

                continue;
            }
        }

        CLT_DATA curr_user;

        curr_user.ip = clt_ip;
        curr_user.port = clt_port;
        curr_user.sock = clt_sock;

        clt_list.push_back(curr_user);

        std::cout << get_format_time('3') << ' ' << clt_ip << ':' << clt_port << " | Login successfully\n";

        logger(file_log ,get_format_time('3') + ' ' + clt_ip + std::to_string(clt_port) + " | Login successfully\n");
    }

    logger(file_log, get_format_time('3') + " Server Ended\n");

    closesocket(srv_sock);
    WSACleanup();

    return 0;
}