#include <iostream>
#include <string>
#include <cstdlib>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

void clear_screen (const char& choice) {
    switch (choice) {
        case 1:
            system("cls");
            break;
        case 2:
            system("pause");
            system("cls");
            break;
        case 3:
            Sleep(750);
            system("cls");
            break;
        default:
            std::cout << "Error clean screen\n";
    }
}

int main () {
    WSADATA win_sock_api;

    const int start_up =  WSAStartup(MAKEWORD(2, 2), &win_sock_api);

    if (start_up != 0) {
        return 1;
    }

    SOCKET my_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (my_sock == INVALID_SOCKET) {
        WSACleanup();

        return 1;
    }

    sockaddr_in srv_addr{};

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(8080);
    srv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(my_sock, reinterpret_cast<sockaddr*>(&srv_addr), sizeof(srv_addr)) == SOCKET_ERROR) {
        closesocket(my_sock);
        WSACleanup();

        return 1;
    }

    while (true) {
        int choice;

        std::cout << "------------------(MENU)------------------\n";
        std::cout << " 1.Register\n";
        std::cout << " 2.Login\n";
        std::cout << "------------------------------------------\n";
        std::cout << "Select choice: ";
        std::cin >> choice;

        clear_screen('1');

        switch (choice) {
            case 1:
                {
                    std::string user, pass;

                    std::cout << "------------------(REGISTER)------------------\n";

                    std::cout << "Create you username: ";
                    std::cin >> user;

                    std::cout << "Create you password: ";
                    std::cin >> pass;

                    clear_screen('3');

                    std::string msg = user + '|' + pass;

                    std::string packet = "0/" + msg + "/" + std::to_string(msg.size());

                    int sent = send(my_sock, packet.c_str(), static_cast<int>(packet.size()), 0);

                    if (sent == SOCKET_ERROR) {
                        std::cerr << "Error send failed (" << WSAGetLastError() << ") Socket error or another proplem\n";

                        closesocket(my_sock);
                        WSACleanup();

                        return 1;
                    
                    }

                    char buffer[1024];

                    const int rec = recv(my_sock, buffer, sizeof(buffer) - 1, 0);

                    if (rec > 0) {
                        buffer[rec] = '\0';

                        std::cout << buffer << '\n';
                    } else {
                        std::cerr << "Error recv failed (" << WSAGetLastError() << ") Socket error or another proplem\n";

                        closesocket(my_sock);
                        WSACleanup();

                        return 1;
                    }
                }

                break;
            case 2:
                {
                    std::string user, pass;

                    std::cout << "------------------(LOGIN)------------------\n";

                    std::cout << "Enter you username: ";
                    std::cin >> user;

                    std::cout << "Enter you password: ";
                    std::cin >> pass;

                    clear_screen('3');

                    std::string msg = user + '|' + pass;

                    std::string packet = "1/" + msg + "/" + std::to_string(msg.size());

                    int sent = send(my_sock, packet.c_str(), static_cast<int>(packet.size()), 0);

                    if (sent == SOCKET_ERROR) {
                        std::cerr << "Error send failed (" << WSAGetLastError() << ") Socket error or another proplem\n";

                        closesocket(my_sock);
                        WSACleanup();

                        return 1;
                    }

                    char buffer[1024];

                    const int rec = recv(my_sock, buffer, sizeof(buffer) - 1, 0);

                    if (rec > 0) {
                        buffer[rec] = '\0';

                        std::cout << buffer << '\n';
                    } else {
                        std::cerr << "Error recv failed (" << WSAGetLastError() << ") Socket error or another proplem\n";

                        closesocket(my_sock);
                        WSACleanup();

                        return 1;
                    }
                }

                break;
            default:
                std::cout << "Invalid choice, Please try again\n";
        }
    }

    closesocket(my_sock);
    WSACleanup();

    return 0;
}