#include<cstring>
#include<iostream>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>


#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT 8080

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

    int ListenSocket = 0;
    int ClientSocket = 0;

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Create a socket for the server to listen for client connections.
    ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ListenSocket < 0) {
        std::cerr << "Socket creation failed with error\n";
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(DEFAULT_PORT);

    // Bind the socket to an IP address and port
    if (bind(ListenSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed with error\n";
        close(ListenSocket);
        return 1;
    }

    // Listen for incoming connections
    if (listen(ListenSocket, 5) < 0) {
        std::cerr << "Listen failed with error\n";
        close(ListenSocket);
        return 1;
    }

    while (true) {
        std::cout << "Waiting for client connection...\n";

        // Accept a client socket
        ClientSocket = accept(ListenSocket, (struct sockaddr*)&client_addr, &client_addr_size);
        if (ClientSocket < 0) {
            std::cerr << "Accept failed with error\n";
            close(ListenSocket);
            return 1;
        }

        // Receive data from the client
        int N = 0;
        char* word = nullptr;
        int c = 0;

        
        int iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            std::cout << "Bytes received: " << iResult << "\n";
            recvbuf[iResult] = '\0';
            std::cout << "Message received: " << recvbuf << "\n";

            int letters = 0, nums = 0;
            char* wordBuffer = new char[256];
            char* numBuffer = new char[256];
            for (int i = 0; i < iResult; i++) {
                if (std::isalpha(recvbuf[i])) {
                    wordBuffer[letters] = recvbuf[i];
                    letters++;
                }
                else {
                    numBuffer[nums] = recvbuf[i];
                    nums++;
                }
            }
            wordBuffer[letters] = '\0';
            char* number = new char[nums];
            for (int i = 0; i < nums; i++) {
                number[i] = numBuffer[i];
            }
            N = atoi(number);
            std::cout << "Word: " << wordBuffer << "\n";
            std::cout << "Number: " << N << "\n";
            delete[] number;

            
            //word = new char[letters];
            //for (int i = 0; i < letters i++) {
            //    word[i] = recvbuf[i];
            //}
            //word[iResult] = '\0';
            char* codedWord = code(wordBuffer, N, letters);
            if (codedWord == nullptr)
                continue;

            std::cout << "Coded word: " << codedWord << "\n";

            int iSendResult = send(ClientSocket, codedWord, strlen(codedWord), 0);
            if (iSendResult < 0) {
                std::cerr << "Send failed with error\n";
                close(ClientSocket);
                return 1;
            }


            std::cout << "Bytes sent: " << iSendResult << "\n";
            std::cout << "---------------------\n";
        } else if (iResult == 0) {
            std::cout << "Connection closing...\n";
        } else {
            std::cerr << "Recv failed with error\n";
            close(ClientSocket);
            return 1;
        }
    }


    // Shutdown the connection
    int iResult = shutdown(ClientSocket, SHUT_RDWR);
    if (iResult < 0) {
        std::cerr << "Shutdown failed with error\n";
        close(ClientSocket);
        return 1;
    }
    close(ClientSocket);

    close(ListenSocket);
    return 0;
}