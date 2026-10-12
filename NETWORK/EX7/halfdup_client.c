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
    struct sockaddr_in server;
    socklen_t addr_len = sizeof(server);
    char line[LINE_SIZE];
    char batch[BATCH_SIZE];
    char packet[PACKET_SIZE];
    int batch_len = 0;
    int my_turn = 1;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    batch[0] = '\0';

    while (1) {
        if (my_turn) {
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
                       (struct sockaddr *)&server, sizeof(server));
                printf("You left the chat.\n");
                break;
            }

            if (strcmp(line, "over") == 0) {
                packet[0] = 'M';
                strncpy(packet + 1, batch, BATCH_SIZE);
                packet[PACKET_SIZE - 1] = '\0';
                sendto(sockfd, packet, strlen(packet), 0,
                       (struct sockaddr *)&server, sizeof(server));

                batch[0] = '\0';
                batch_len = 0;
                my_turn = 0;
                printf("Waiting for server...\n");
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

        } else {
            n = recvfrom(sockfd, packet, PACKET_SIZE - 1, 0, NULL, NULL);
            if (n < 0) {
                perror("recvfrom");
                break;
            }
            packet[n] = '\0';

            if (packet[0] == 'E') {
                if (strlen(packet + 1) > 0)
                    printf("%s", packet + 1);
                printf("Server left the chat.\n");
                break;
            } else if (packet[0] == 'M') {
                if (strlen(packet + 1) > 0)
                    printf("%s", packet + 1);
                my_turn = 1;
            } else {
                printf("%s\n", packet);
                my_turn = 1;
            }
        }
    }

    close(sockfd);
    return 0;
}
