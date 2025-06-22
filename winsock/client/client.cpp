#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;


int main() {
    WSADATA wsaData;
    // 1. Инициализация WinSock 2.2
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // 2. Создание TCP-сокета
    SOCKET clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // 3. Указание адреса сервера для подключения
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1234);             // порт сервера (сетевой порядок байтов)
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);  // IP адрес сервера (localhost)
    // Для старых версий можно использовать: serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 4. Подключение к серверу
    if (connect(clientSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSock);
        WSACleanup();
        return 1;
    }
    std::cout << "Connected to server." << std::endl;

    char buf[1024] = { 0 };

    while (1)
    {
        cout << "Please, enter message or 'exit' to exit: " << endl;
        string str;
        getline(cin, str);
        if (str == "exit")
        {
            closesocket(clientSock);
            WSACleanup();
            exit(1);
        }

        int bytesSent = send(clientSock, str.c_str(), (int)strlen(str.c_str()), 0);
        if (bytesSent == SOCKET_ERROR) {
            cerr << "Send failed: " << WSAGetLastError() <<endl;
            closesocket(clientSock);
            WSACleanup();
            return 1;
        }
    }


    //// 5. Отправка сообщения серверу
    //const char* msg = "Hello from client";
    //int bytesSent = send(clientSock, msg, (int)strlen(msg), 0);
    //if (bytesSent == SOCKET_ERROR) {
    //    std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
    //    closesocket(clientSock);
    //    WSACleanup();
    //    return 1;
    //}

    //// 6. Чтение ответа от сервера
    //char buf[256] = { 0 };
    //int bytesRecv = recv(clientSock, buf, sizeof(buf) - 1, 0);
    //if (bytesRecv > 0) {
    //    buf[bytesRecv] = '\0';
    //    std::cout << "Received from server: " << buf << std::endl;
    //}
    //else if (bytesRecv == 0) {
    //    std::cout << "Connection closed by server." << std::endl;
    //}
    //else {
    //    std::cerr << "Recv failed: " << WSAGetLastError() << std::endl;
    //}

    // 7. Закрытие сокета и очистка WinSock
    closesocket(clientSock);
    WSACleanup();
    return 0;
}
