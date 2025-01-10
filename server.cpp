// Copyright 2025 metaarsenit
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include<iostream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "8080"

// Cesar algorithm
char* code(const char* word, int N, int length) {
    if (word == nullptr)
        return nullptr;

    char* answer = new char[length+1];
    for (int i = 0; i < length; i++) {
        int value = static_cast<int>(word[i]);
        // 65-90 A-Z
        if (65 <= value && value <= 90) {
            answer[i] = ((value - 65 + N) % 26) + 65;
        } else if (97 <= value && value <= 122) {
            // 97-122 a-z
            answer[i] = ((value - 97 + N) % 26) + 97;
        } else {
            // special characters are unchanged
            answer[i] = static_cast<char>(value);
        }
    }
    answer[length] = '\0';
    return answer;
}

// https://learn.microsoft.com/ru-ru/windows/win32/winsock/finished-server-and-client-code
int main() {
    std::cout << "Starting server...\n";

    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed with error: " << iResult << "\n";
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed with error: " << iResult << "\n";
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family,
                          result->ai_socktype,
                          result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::cout << "socket failed with error: ";
        std::cout << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket,
                   result->ai_addr,
                   static_cast<int>(result->ai_addrlen));
    if (iResult == SOCKET_ERROR) {
        std::cout << "bind failed with error: ";
        std::cout << WSAGetLastError() << "\n";
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cout << "listen failed with error: ";
        std::cout << WSAGetLastError() << "\n";
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }



    while (true) {
        std::cout << "Waiting for client connection...\n";

        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            std::cout << "accept failed with error: ";
            std::cout << WSAGetLastError() << "\n";
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        // Receive until the peer shuts down the connection
        int N = 0;
        char* word = nullptr;
        int c = 0;

        do {
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            if (iResult > 0) {
                std::cout << "Bytes received: " << iResult << "\n";
                recvbuf[iResult] = '\0';
                std::cout << "Message received: " << recvbuf << "\n";
                // Echo the buffer back to the sender
                if (c == 0) {
                    word = new char[iResult+1];
                    for (int i = 0; i < iResult; i++) {
                        word[i] = recvbuf[i];
                    }
                    word[iResult] = '\0';
                }

                if (c == 1) {
                    char* number = new char[iResult];
                    for (int i = 0; i < iResult; i++) {
                        number[i] = recvbuf[i];
                    }
                    int N = atoi(number);
                    delete[] number;

                    char* codedWord = code(word, N, strlen(word));
                    if (codedWord == nullptr)
                        break;

                    std::cout << "Coded word: " << codedWord << "\n";

                    iSendResult = send(ClientSocket,
                                       codedWord,
                                       strlen(codedWord), 0);
                    if (iSendResult == SOCKET_ERROR) {
                        std::cout << "send failed with error: ";
                        std::cout << WSAGetLastError() << "\n";
                        closesocket(ClientSocket);
                        WSACleanup();
                        return 1;
                    }

                    std::cout << "Bytes sent: ";
                    std::cout << iSendResult << "\n";
                }

                c++;
                std::cout << "---------------------\n";
            } else if (iResult == 0) {
                std::cout << "Connection closing...\n";
            } else {
                std::cout << "recv failed with error: ";
                std::cout << WSAGetLastError() << "\n";
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
        } while (iResult > 0);

        delete[] word;
        std::cout << "\n\n";
        // shutdown the connection
        iResult = shutdown(ClientSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            std::cout << "shutdown failed with error: ";
            std::cout << WSAGetLastError() << "\n";
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    }

    closesocket(ListenSocket);
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}
