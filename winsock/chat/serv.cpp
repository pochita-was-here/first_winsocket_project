#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main() {

    char buff[1024] = { 0 };

    WSADATA wsaData;
    // 1. Инициализация WinSock версии 2.2
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        exit(1);
    }

    // 2. Создание TCP-сокета (IPv4)
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    // 3. Привязка сокета к локальному адресу и порту
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1234);             // порт 1234 (в сетевом порядке байтов)
    serverAddr.sin_addr.s_addr = INADDR_ANY;        // слушать на всех интерфейсах (0.0.0.0)
    
    bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // 4. Прослушивание входящих подключений
    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {           // в параметрах первым сокет, вторым очередь подключений, SOMAXCONN это максимальная очередь
        std::cerr << "Listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
    cout << "Waiting for connection..." << endl;

    // 5. Принятие подключения от клиента
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrSize); // блокирующая функция ожидает входящего соединения, возарщает новый сокет clientSock для общения или инвалид
    // обычно передаётся пустая структура sockaddr_in для получения адреса клиента, можно передать NULL вместо адреса если детали клиента не нужны
    // после accept() сервер продолжает слушать на исходном listenSock (можно снова вызывать accept для новых клиентов)
    // я закрываю listenSock потому что жду только одного клиента
    if (clientSock == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
    std::cout << "Client connected!" << std::endl;
    closesocket(listenSock);  // больше не нужен, есть clientSock
    
    string msg;

    while (1)
    {
        memset(buff, 0, sizeof(buff)); // обнуляю буфер
        msg = "";

        int bytesRecv = recv(clientSock, buff, sizeof(buff) - 1, 0);
        if (bytesRecv > 0)
        {
            buff[bytesRecv] = '\0'; // чтобы можно было работать как со строкой
            msg = buff;
            if (strcmp(buff, "exit") == 0)
            {
                exit(1);
            }
            cout << "Recieved message: " << msg << endl;
        }
        else if (bytesRecv == 0)
        {
            cout << "Client disconnected, exiting..." << endl;
            closesocket(clientSock);
            WSACleanup();
            exit(1);
        }
        else if (bytesRecv < 0)
        {
            cerr << "Recv failed: " << WSAGetLastError() << endl;
            closesocket(clientSock);
            WSACleanup();
            exit(-1);
        }

        int bytesSent = send(clientSock, msg.c_str(), (int)strlen(msg.c_str()), 0);
        if (bytesSent == SOCKET_ERROR)
        {
            cerr << "Send failed: " << WSAGetLastError() << endl; 
        }

    }

    //// 6. Получение данных от клиента
    //char buf[256] = { 0 };
    //int bytesRecv = recv(clientSock, buf, sizeof(buf) - 1, 0);
    //if (bytesRecv > 0) {
    //    buf[bytesRecv] = '\0';  // делаем буфер строкой для вывода
    //    std::cout << "Received from client: " << buf << std::endl;
    //}

    //// 6. Отправка ответа клиенту
    //const char* reply = "Hello from server";
    //int bytesSent = send(clientSock, reply, (int)strlen(reply), 0);
    //if (bytesSent == SOCKET_ERROR) {
    //    std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
    //}

    // 7. Закрытие соединения и очистка WinSock
    closesocket(clientSock);
    WSACleanup();
    return 0;
}
