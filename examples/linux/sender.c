// This code is not protected by any copyright as far as I know
// It is based on misc public sources on the Internet
// Prepared by Mark Polyak in 2020

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>

char msg1[] = "Hello there!\n";
char msg2[] = "Bye bye!\n";

int main() {
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sendto(sock, msg1, sizeof(msg1), 0,
           (struct sockaddr *)&addr, sizeof(addr));

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    send(sock, msg2, sizeof(msg2), 0);

    close(sock);

    return 0;
}
