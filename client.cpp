#define SYN_TO_SERVER "de7d1b721a1e0632b7cf04edf5032c8ecffa9f9a08492152b926f1a5a7e765d7" // i need connect
#define SYN_ACK_TO_CLIENT "a7722b765d15608d5d24a319140e6665cb361a0758285125722a2d5aece38808" // well come to server
#define ACK_TO_SERVER "ab6db599234d2636659cba1aa191bd014c3867d5cfade98ff694785c20c28fc6" // am connect success
#define LOGIN_RESPONSE "428821350e9691491f616b754cd8315fb86d797ab35d843479e732ef90665324" // login success

#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "packet.h"
#include "logger.h"
#include "time.h"

const std::string file_log = "client.log";

#pragma comment(lib, "ws2_32.lib")

int main () {
    WSADATA win_sock_api;

    const int start_up = WSAStartup(MAKEWORD(2, 2), &win_sock_api);

    if (start_up != 0) {
        // std::cerr << "Error code (" << start_up << ") Start winsock api failed\n";

        logger(file_log, get_format_time('3') + " Error code (" + std::to_string(start_up) + ") Start winsock api failed\n");

        return 1;
    }

    SOCKET srv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (srv_sock == INVALID_SOCKET) {
        // std::cerr << "Error code (" << WSAGetLastError() << ") Socket creation failed\n";

        logger(file_log, get_format_time('3') + " Error code (" + std::to_string(WSAGetLastError()) + ") Socket creation failed\n");

        WSACleanup();

        return 1;
    }

    sockaddr_in srv_addr{};

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(8080);
    srv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(srv_sock, reinterpret_cast<sockaddr*>(&srv_addr), sizeof(srv_addr)) == INVALID_SOCKET) {
        // std::cerr << "Error code (" << WSAGetLastError() << ") Connected to server failed\n";

        logger(file_log, get_format_time('3') + " Error code (" + std::to_string(WSAGetLastError()) + ") Connected to server failed\n");

        closesocket(srv_sock);
        WSACleanup();
            
        return 1;
    }
        
    {
        const std::string packet = "SYN/" + std::string(SYN_TO_SERVER) + "/" + std::to_string(std::string(SYN_TO_SERVER).size());
        const int sent = sent_data(srv_sock, packet.c_str(), packet.size());

        // std::cout << "Send packet size: " << packet.size() << '\n';

        if (sent == SOCKET_ERROR) {
            // std::cerr << "Send SYN to server failed\n";

            logger(file_log, get_format_time('3') + "Send SYN to server failed\n");

            closesocket(srv_sock);
            WSACleanup();

            return 1;
        }else if (sent == 0) {
            std::cerr << "Server offline\n";

            logger(file_log, get_format_time('3') + "Server offline\n");

            closesocket(srv_sock);
            WSACleanup();

            return 1;
        }
    }

    {
        char buffer[75]{};
        const int rece = recv_data(srv_sock, buffer, sizeof(buffer));

        if (rece == SOCKET_ERROR) {
            // std::cerr << "Recv SYN-ACK from server failed\n";

            logger(file_log, get_format_time('3') + "Recv SYN-ACK from server failed\n");
                
            closesocket(srv_sock);
            WSACleanup();

            return 1;
        } else if (rece == 0) {
            // std::cerr << "Server offline\n";

            logger(file_log, get_format_time('3') + "Server offline\n");
                
            closesocket(srv_sock);
            WSACleanup();

            return 1;
        } else {
            std::string temp_buffer = buffer;

            size_t p1 = temp_buffer.find('/');
            size_t p2 = temp_buffer.find('/', p1 + 1);

            if (p1 == std::string::npos || p2 == std::string::npos) {
                // std::cerr << " Invalid packet format\n";

                logger(file_log, get_format_time('3') + " Invalid packet format\n");

                closesocket(srv_sock);

                return 1;
            }

            // type/data/size

            const std::string type = temp_buffer.substr(0, p1);
            std::string data = temp_buffer.substr(p1 + 1, p2 - p1 - 1);
            int size = std::stoi(temp_buffer.substr(p2 + 1));

            if (type != "SYN-ACK" || data != SYN_ACK_TO_CLIENT) {
                // std::cerr << "Error packet ACK\n";

                logger(file_log, get_format_time('3') + "Error packet ACK\n");

                closesocket(srv_sock);

                return 1;
            }
        }
    }

    {
        const std::string packet = "ACK/" + std::string(ACK_TO_SERVER) + "/" + std::to_string(std::string(ACK_TO_SERVER).size());
        const int sent = sent_data(srv_sock, packet.c_str(), packet.size());

        // std::cout << "Send packet size: " << packet.size() << '\n';

        if (sent == SOCKET_ERROR) {
            // std::cerr << "Send ACK to server failed\n";

            logger(file_log, get_format_time('3') + "Send ACK to server failed\n");

            closesocket(srv_sock);
            WSACleanup();

            return 1;
        }else if (sent == 0) {
            // std::cerr << "Server offline\n";

            logger(file_log, get_format_time('3') + "Server offline\n");
            
            closesocket(srv_sock);
            WSACleanup();

            return 1;
        }
    }

    std::cout << "Successfully connected to the server\n";

    logger(file_log, get_format_time('3') + " Successfully connected to the server\n");

    const int max_attempt = 1;

    for (int i = 0; i < max_attempt; i++) {
        std::string username, password;

        std::cin >> username;
        std::cin >> password;

        const std::string data = username + '|' + password;
        const std::string packet = "LOGIN-REQUEST/" + data + "/" + std::to_string(data.size());
        const int sent = send(srv_sock, packet.c_str(), packet.size(), 0);

        // std::cout << "Send packet size: " << packet.size() << '\n';

        if (sent == SOCKET_ERROR) {
            // std::cerr << "Send login request failed\n";

            logger(file_log, get_format_time('3') + "Send login request failed\n");

            closesocket(srv_sock);
            WSACleanup();

            return 1;
        }else if (sent == 0) {
            // std::cerr << "Server offline\n";

            logger(file_log, get_format_time('3') + "Server offline\n");
            
            closesocket(srv_sock);
            WSACleanup();

            return 1;
        }
        
    }

    {
        char buffer[1024]{};
        const int rece = recv(srv_sock, buffer, sizeof(buffer), 0);

        if (rece == SOCKET_ERROR) {
            // std::cerr << "Recv login response from server failed\n";

            logger(file_log, get_format_time('3') + "Recv login response from server failed\n");
                
            closesocket(srv_sock);
            WSACleanup();

            return 1;
        } else if (rece == 0) {
            // std::cerr << "Server offline\n";

            logger(file_log, get_format_time('3') + "Server offline\n");
                
            closesocket(srv_sock);
            WSACleanup();

            return 1;
        } else {
            std::string temp_buffer = buffer;

            size_t p1 = temp_buffer.find('/');
            size_t p2 = temp_buffer.find('/', p1 + 1);

            if (p1 == std::string::npos || p2 == std::string::npos) {
                // std::cerr << " Invalid packet format\n";

                logger(file_log, get_format_time('3') + " Invalid packet format\n");

                closesocket(srv_sock);

                return 1;
            }

            // type/data/size

            const std::string type = temp_buffer.substr(0, p1);
            std::string data = temp_buffer.substr(p1 + 1, p2 - p1 - 1);
            int size = std::stoi(temp_buffer.substr(p2 + 1));

            if (type != "LOGIN_RESPONSE" || data != LOGIN_RESPONSE) {
                // std::cerr << "Error packet login response\n";

                logger(file_log, get_format_time('3') + "Error packet login response\n");

                closesocket(srv_sock);

                return 1;
            }
        }
    }

    std::cout << "Login successfully\n";

    logger(file_log, get_format_time('3') + " Login successfully\n");

    logger(file_log, get_format_time('3') + " Client Ended\n");

    closesocket(srv_sock);
WSACleanup();
        
return 0;
}