#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5090
#define BUFSIZE 1024

/* socket + connect -- returns the connected socket */
int connectTCP(const char *ip) {
    int sock;
    struct sockaddr_in server_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); exit(1); }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address\n"); exit(1);
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect"); exit(1);
    }
    printf("Connected to server.\n");
    return sock;
}

void displayMenu() {
    printf("\n===== TCP CLIENT MENU =====\n");
    printf("1. Echo\n");
    printf("2. Palindrome\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
}

void handleEcho(int sock) {
    char msg[BUFSIZE / 2], request[BUFSIZE], response[BUFSIZE];
    int n;

    printf("Enter message: ");
    fgets(msg, sizeof(msg), stdin);
    msg[strcspn(msg, "\n")] = '\0';

    snprintf(request, BUFSIZE, "1|%s", msg);
    send(sock, request, strlen(request), 0);

    n = recv(sock, response, BUFSIZE - 1, 0);
    if (n <= 0) { printf("Server closed connection.\n"); exit(1); }
    response[n] = '\0';
    printf("Echo from server: %s\n", response);
}

void handlePalindrome(int sock) {
    char msg[BUFSIZE / 2], request[BUFSIZE], response[BUFSIZE];
    int n;

    printf("Enter a string: ");
    fgets(msg, sizeof(msg), stdin);
    msg[strcspn(msg, "\n")] = '\0';

    snprintf(request, BUFSIZE, "2|%s", msg);
    send(sock, request, strlen(request), 0);

    n = recv(sock, response, BUFSIZE - 1, 0);
    if (n <= 0) { printf("Server closed connection.\n"); exit(1); }
    response[n] = '\0';
    printf("Server: %s\n", response);
}

int main(int argc, char *argv[]) {
    const char *ip = (argc > 1) ? argv[1] : "127.0.0.1";
    int sock, choice;

    sock = connectTCP(ip);

    while (1) {
        displayMenu();
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); continue; }
        getchar();

        switch (choice) {
            case 1:
                handleEcho(sock);
                break;
            case 2:
                handlePalindrome(sock);
                break;
            case 3:
                send(sock, "3|", 2, 0);     /* tell server to terminate */
                printf("Exiting...\n");
                close(sock);
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}
