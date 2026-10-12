#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT        8082
#define LINE_SIZE   1024
#define BATCH_SIZE  4096
#define PACKET_SIZE (BATCH_SIZE + 2)

int main() {
    int sockfd, n;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);
    char line[LINE_SIZE];
    char batch[BATCH_SIZE];
    char packet[PACKET_SIZE];
    int batch_len = 0;
    int my_turn = 0;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    printf("UDP Half-Duplex Server waiting...\n");

    batch[0] = '\0';

    while (1) {
        if (!my_turn) {
            client_len = sizeof(client);
            n = recvfrom(sockfd, packet, PACKET_SIZE - 1, 0,
                         (struct sockaddr *)&client, &client_len);
            if (n < 0) {
                perror("recvfrom");
                continue;
            }
            packet[n] = '\0';

            if (packet[0] == 'E') {
                if (strlen(packet + 1) > 0)
                    printf("%s", packet + 1);
                printf("Client left the chat.\n");
                break;
            } else if (packet[0] == 'M') {
                if (strlen(packet + 1) > 0)
                    printf("%s", packet + 1);
                my_turn = 1;
            } else {
                printf("%s\n", packet);
                my_turn = 1;
            }

        } else {
            printf("[You] ");
            fflush(stdout);

            if (fgets(line, sizeof(line), stdin) == NULL)
                break;
            line[strcspn(line, "\n")] = '\0';

            if (strcmp(line, "exit") == 0) {
                packet[0] = 'E';
                strncpy(packet + 1, batch, BATCH_SIZE);
                packet[PACKET_SIZE - 1] = '\0';
                sendto(sockfd, packet, strlen(packet), 0,
                       (struct sockaddr *)&client, client_len);
                printf("You left the chat.\n");
                break;
            }

            if (strcmp(line, "over") == 0) {
                packet[0] = 'M';
                strncpy(packet + 1, batch, BATCH_SIZE);
                packet[PACKET_SIZE - 1] = '\0';
                sendto(sockfd, packet, strlen(packet), 0,
                       (struct sockaddr *)&client, client_len);

                batch[0] = '\0';
                batch_len = 0;
                my_turn = 0;
                printf("Waiting for client...\n");
                continue;
            }

            size_t line_len = strlen(line);
            if (batch_len + line_len + 1 >= BATCH_SIZE) {
                printf("Batch full! Type 'over' to send.\n");
            } else {
                strcat(batch, line);
                strcat(batch, "\n");
                batch_len += line_len + 1;
            }
        }
    }

    close(sockfd);
    return 0;
}
