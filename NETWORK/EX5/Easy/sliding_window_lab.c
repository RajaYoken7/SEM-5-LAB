
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_SEQ 15
#define WINDOW_SIZE 4
#define TOTAL_FRAMES 10
#define PROB_LOSS 30  // Probability of frame loss/ACK loss (percentage)

int sent[MAX_SEQ + 1];
int acked[MAX_SEQ + 1];

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pauseScreen() {
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

int lossSimulation() {
    return (rand() % 100) < PROB_LOSS;
}

// ===================== STOP AND WAIT =====================
void stopAndWait() {
    int totalFrames, i;
    int ackReceived;

    clearScreen();
    printf("========================================\n");
    printf("       STOP AND WAIT ARQ PROTOCOL       \n");
    printf("========================================\n\n");

    printf("Enter total number of frames to send: ");
    scanf("%d", &totalFrames);

    srand(time(NULL));

    for (i = 0; i < totalFrames; i++) {
        sent[i] = 0;
        acked[i] = 0;
    }

    printf("\n--- Starting Stop and Wait Simulation ---\n");
    printf("Window Size: 1 | Total Frames: %d\n\n", totalFrames);

    for (i = 0; i < totalFrames; i++) {
        ackReceived = 0;

        while (!ackReceived) {
            printf("[SENDER] Sending frame %d...\n", i);
            usleep(400000); // simulate transmission delay

            if (lossSimulation()) {
                printf("[NETWORK] Frame %d or its ACK was LOST!\n", i);
                printf("[SENDER] Timeout! Retransmitting frame %d...\n\n", i);
                usleep(400000);
            } else {
                printf("[RECEIVER] Frame %d received OK. Sending ACK %d\n", i, i);
                printf("[SENDER] ACK %d received successfully.\n\n", i);
                acked[i] = 1;
                ackReceived = 1;
            }
        }

        usleep(300000);
    }

    printf("========================================\n");
    printf(" All frames sent successfully (Stop & Wait)!\n");
    printf("========================================\n");
    pauseScreen();
}

// ===================== GO-BACK-N =====================
void goBackN() {
    int totalFrames, windowSize, i, base = 0, nextSeq = 0;
    int ack;

    clearScreen();
    printf("========================================\n");
    printf("         GO-BACK-N ARQ PROTOCOL         \n");
    printf("========================================\n\n");

    printf("Enter total number of frames to send: ");
    scanf("%d", &totalFrames);
    printf("Enter window size: ");
    scanf("%d", &windowSize);

    srand(time(NULL));

    for (i = 0; i < totalFrames; i++) {
        sent[i] = 0;
        acked[i] = 0;
    }

    printf("\n--- Starting Go-Back-N Simulation ---\n");
    printf("Window Size: %d | Total Frames: %d\n\n", windowSize, totalFrames);

    while (base < totalFrames) {
        // Send frames in the window
        for (i = base; i < base + windowSize && i < totalFrames; i++) {
            if (!sent[i] || (i >= base && !acked[i])) {
                printf("[SENDER] Sending frame %d...\n", i);
                usleep(300000); // simulate delay
                sent[i] = 1;
            }
        }

        printf("\n");

        // Simulate ACKs for frames in window
        int problem = -1;
        for (i = base; i < base + windowSize && i < totalFrames; i++) {
            if (lossSimulation()) {
                printf("[NETWORK] Frame %d or its ACK was LOST!\n", i);
                problem = i;
                break; // In GBN, once a frame is lost, subsequent frames are discarded
            } else {
                printf("[RECEIVER] Frame %d received OK. Sending ACK %d\n", i, i);
                acked[i] = 1;
            }
        }

        printf("\n");

        if (problem != -1) {
            printf("[SENDER] Timeout! Resending from frame %d onwards...\n\n", problem);
            for (int j = problem; j < base + windowSize && j < totalFrames; j++) {
                sent[j] = 0;
                acked[j] = 0;
            }
        } else {
            printf("[SENDER] All frames in window acknowledged. Sliding window forward.\n\n");
            base += windowSize;
        }

        usleep(500000);
    }

    printf("========================================\n");
    printf("  All frames sent successfully (GBN)!   \n");
    printf("========================================\n");
    pauseScreen();
}

// ===================== SELECTIVE REPEAT =====================
void selectiveRepeat() {
    int totalFrames, windowSize, i, base = 0;
    int allAcked;

    clearScreen();
    printf("========================================\n");
    printf("      SELECTIVE REPEAT ARQ PROTOCOL    \n");
    printf("========================================\n\n");

    printf("Enter total number of frames to send: ");
    scanf("%d", &totalFrames);
    printf("Enter window size: ");
    scanf("%d", &windowSize);

    srand(time(NULL));

    for (i = 0; i < totalFrames; i++) {
        sent[i] = 0;
        acked[i] = 0;
    }

    printf("\n--- Starting Selective Repeat Simulation ---\n");
    printf("Window Size: %d | Total Frames: %d\n\n", windowSize, totalFrames);

    while (1) {
        // Check if all frames are acked
        allAcked = 1;
        for (i = 0; i < totalFrames; i++) {
            if (!acked[i]) {
                allAcked = 0;
                break;
            }
        }
        if (allAcked) break;

        // Send frames in current window that are not yet acked
        for (i = base; i < base + windowSize && i < totalFrames; i++) {
            if (!sent[i]) {
                printf("[SENDER] Sending frame %d...\n", i);
                usleep(300000);
                sent[i] = 1;
            }
        }

        printf("\n");

        // Simulate individual ACKs/NACKs
        for (i = base; i < base + windowSize && i < totalFrames; i++) {
            if (!acked[i]) {
                if (lossSimulation()) {
                    printf("[NETWORK] Frame %d or its ACK was LOST! Will retransmit individually.\n", i);
                } else {
                    printf("[RECEIVER] Frame %d received OK. Sending ACK %d\n", i, i);
                    acked[i] = 1;
                }
            }
        }

        printf("\n");

        // Slide window base to first unacked frame
        while (base < totalFrames && acked[base]) {
            base++;
        }

        // Retransmit lost frames individually
        for (i = base; i < base + windowSize && i < totalFrames; i++) {
            if (sent[i] && !acked[i]) {
                printf("[SENDER] Retransmitting lost frame %d individually...\n", i);
                usleep(300000);
                if (lossSimulation()) {
                    printf("[NETWORK] Frame %d lost again!\n", i);
                } else {
                    printf("[RECEIVER] Frame %d received OK. Sending ACK %d\n", i, i);
                    acked[i] = 1;
                }
            }
        }

        printf("\n[STATUS] Window base now at frame %d\n\n", base);
        usleep(500000);
    }

    printf("========================================\n");
    printf("All frames sent successfully (Selective Repeat)!\n");
    printf("========================================\n");
    pauseScreen();
}

// ===================== MAIN MENU =====================
void mainMenu() {
    int choice;

    do {
        clearScreen();
        printf("========================================\n");
        printf("     NETWORK LABORATORY SIMULATION      \n");
        printf("        SLIDING WINDOW PROTOCOLS          \n");
        printf("========================================\n");
        printf("  1. Stop and Wait ARQ\n");
        printf("  2. Go-Back-N ARQ\n");
        printf("  3. Selective Repeat ARQ\n");
        printf("  0. Exit\n");
        printf("========================================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                stopAndWait();
                break;
            case 2:
                goBackN();
                break;
            case 3:
                selectiveRepeat();
                break;
            case 0:
                printf("\nExiting... Thank you!\n");
                break;
            default:
                printf("\nInvalid choice! Please try again.\n");
                pauseScreen();
        }
    } while (choice != 0);
}

int main() {
    mainMenu();
    return 0;
}
