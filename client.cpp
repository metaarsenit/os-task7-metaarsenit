// Copyright 2025 metaarsenit
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cstring>
#include<string>
#include<iostream>

char* decode(const char* word, int N, int length) {
    if (word == nullptr)
        return nullptr;

    char* answer = new char[length + 1];
    for (int i = 0; i < length; i++) {
        int value = static_cast<int>(word[i]);
        // 65-90 A-Z
        if (65 <= value && value <= 90) {
            answer[i] = (abs((value - 65 + N * 25) % 26)) + 65;
        } else if (97 <= value && value <= 122) {
            // 97-122 a-z
            answer[i] = (abs((value - 97 + N * 25) % 26)) + 97;
        } else {
            // special characters are unchanged
            answer[i] = static_cast<char>(value);
        }
    }
    answer[length] = '\0';
    return answer;
}


using std::cout, std::cin;
using std::string, std::to_string;

int main(int argc, char* argv[]) {
    cout << "Starting client\n";

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = inet_addr(argv[1]);

    cout << "Connecting to the server...\n";
    connect(clientSocket,
            (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));
    cout << "Connection established\n";

    cout << "Type your message [English]: ";
    string _message;
    cin >> _message;
    cout << "Offset [integer]: ";
    unsigned int _offset;
    cin >> _offset;

    const char* offset = to_string(_offset).c_str();
    cout << "Sending to server...\n";
    const char* message = _message.c_str();
    send(clientSocket, message, strlen(message), 0);
    send(clientSocket, offset, strlen(offset), 0);

    int buflen = 1024;
    char recvbuf[1024];

    int iResult = recv(clientSocket, recvbuf, buflen, 0);
    if (iResult > 0) {
        cout << "Bytes received: " << iResult << "\n";
        recvbuf[iResult] = '\0';
        cout << "Message received: " << recvbuf  << "\n";
        cout << "Decoded back: ";
        cout << decode(recvbuf, _offset, strlen(recvbuf)) << "\n";
    }
    close(clientSocket);
    cout << "Socket closed\n";

    return 0;
}
