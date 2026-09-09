[24bcs056@mepcolinux ex5]$cat semaphore.h
#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

#define SIZE 5      // Define your circular buffer size here
#define MUTEX 0     // Semaphore index 0 for Critical Section
#define FULL 1      // Semaphore index 1 for Full Slots
#define EMPTY 2     // Semaphore index 2 for Empty Slots

// System V semaphore control union
union semaphore {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void wait_sem(int sid, int sno);
void signal_sem(int sid, int sno);

#endif
[24bcs056@mepcolinux ex5]$cat imp.c
#include "semaphore.h"

// Atomic down operation (P)
void wait_sem(int sid, int sno) {
    struct sembuf s;
    s.sem_num = sno;
    s.sem_op = -1;
    s.sem_flg = SEM_UNDO;
    if (semop(sid, &s, 1) == -1) {
        perror("Wait operation failed");
        exit(1);
    }
}

// Atomic up operation (V)
void signal_sem(int sid, int sno) {
    struct sembuf s;
    s.sem_num = sno;
    s.sem_op = 1;
    s.sem_flg = SEM_UNDO;
    if (semop(sid, &s, 1) == -1) {
        perror("Signal operation failed");
        exit(1);
    }
}
[24bcs056@mepcolinux ex5]$cat prod.c
#define _DEFAULT_SOURCE
#include "semaphore.h"

int main() {
    union semaphore snum;
    int sid, shmid, num, in = 0;
    int *buf;
    unsigned short a[3]; // Fixed initialization array syntax

    key_t shm_key = ftok(".", 'A');
    key_t sem_key = ftok(".", 'B');

    // 1. Create Shared Memory Segment
    shmid = shmget(shm_key, sizeof(int) * SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Producer: shmget failed");
        exit(1);
    }

    // Attach to Shared Memory block
    buf = (int *)shmat(shmid, NULL, 0);
    if (buf == (void *)-1) {
        perror("Producer: shmat failed");
        exit(1);
    }

    // 2. Create Semaphore Array
    sid = semget(sem_key, 3, IPC_CREAT | 0666);
    if (sid == -1) {
        perror("Producer: semget failed");
        exit(1);
    }

    // Setup initial semaphore values
    a[MUTEX] = 1;
    a[FULL] = 0;
    a[EMPTY] = SIZE;
    snum.array = a;
    semctl(sid, 0, SETALL, snum);

    printf("--- Producer Started (Buffer Size: %d) ---\n", SIZE);
    printf("Enter values to produce. Enter -1 to terminate.\n");

    while (1) {
        printf("\nEnter data item: ");
        if (scanf("%d", &num) != 1) break;

        // Synchronize before writing
        wait_sem(sid, EMPTY);
        wait_sem(sid, MUTEX);

        // Write to circular buffer using modulo wrapping
        buf[in % SIZE] = num;
        printf("[Producer] Inserted %d at circular index %d\n", num, in % SIZE);
        in++;

        // Signal completion
        signal_sem(sid, MUTEX);
        signal_sem(sid, FULL);

        // If -1 is submitted, break out of loop to exit cleanly
        if (num == -1) {
            printf("[Producer] Termination value (-1) entered. Exiting.\n");
            break;
        }
    }

    shmdt(buf);
    return 0;
}
[24bcs056@mepcolinux ex5]$cat cons.c
#define _DEFAULT_SOURCE
#include "semaphore.h"

int main() {
    int sid, shmid, num, out = 0;
    int *buf;

    key_t shm_key = ftok(".", 'A');
    key_t sem_key = ftok(".", 'B');

    // 1. Locate Shared Memory Block
    shmid = shmget(shm_key, sizeof(int) * SIZE, 0666);
    if (shmid == -1) {
        perror("Consumer: shmget failed. Run producer first.");
        exit(1);
    }

    buf = (int *)shmat(shmid, NULL, 0);
    if (buf == (void *)-1) {
        perror("Consumer: shmat failed");
        exit(1);
    }

    // 2. Access the Semaphore Array
    sid = semget(sem_key, 3, 0666);
    if (sid == -1) {
        perror("Consumer: semget failed");
        exit(1);
    }

    printf("--- Consumer Started ---\n\n");

    while (1) {
        // Synchronize before reading
        wait_sem(sid, FULL);
        wait_sem(sid, MUTEX);

        // Read from circular buffer using modulo wrapping
        num = buf[out % SIZE];
        printf("[Consumer] Read %d from circular index %d\n", num, out % SIZE);
        out++;

        // Signal that the slot has been consumed and is available for reuse
        signal_sem(sid, MUTEX);
        signal_sem(sid, EMPTY);

        // If data is -1, break loop and perform IPC cleanup
        if (num == -1) {
            printf("[Consumer] Termination signal received. Clearing IPC.\n");
            break;
        }

        sleep(2); // Simulate consumption processing time delay
    }

    // 3. Remove Shared Memory and Semaphores completely from system tables
    shmdt(buf);
    semctl(sid, 0, IPC_RMID);
    shmctl(shmid, IPC_RMID, NULL);

    printf("\nIPC Shared Memory and Semaphore structures cleared successfully.\n");
    return 0;
}
[24bcs056@mepcolinux ex5]$./prod
--- Producer Started (Buffer Size: 5) ---
Enter values to produce. Enter -1 to terminate.

Enter data item: 1
[Producer] Inserted 1 at circular index 0

Enter data item: 2
[Producer] Inserted 2 at circular index 1

Enter data item: 3
[Producer] Inserted 3 at circular index 2

Enter data item: 4
[Producer] Inserted 4 at circular index 3

Enter data item: 5
[Producer] Inserted 5 at circular index 4

Enter data item: 5
[Producer] Inserted 5 at circular index 0

Enter data item: 4
[Producer] Inserted 4 at circular index 1

Enter data item: 3
[Producer] Inserted 3 at circular index 2

Enter data item: 2
[Producer] Inserted 2 at circular index 3

Enter data item: 1
[Producer] Inserted 1 at circular index 4

Enter data item: -1
[Producer] Inserted -1 at circular index 0
[Producer] Termination value (-1) entered. Exiting.

[24bcs056@mepcolinux ex5]$./cons
--- Consumer Started ---

[Consumer] Read 1 from circular index 0
[Consumer] Read 2 from circular index 1
[Consumer] Read 3 from circular index 2
[Consumer] Read 4 from circular index 3
[Consumer] Read 5 from circular index 4
[Consumer] Read 5 from circular index 0
[Consumer] Read 4 from circular index 1
[Consumer] Read 3 from circular index 2
[Consumer] Read 2 from circular index 3
[Consumer] Read 1 from circular index 4
[Consumer] Read -1 from circular index 0
[Consumer] Termination signal received. Clearing IPC.

IPC Shared Memory and Semaphore structures cleared successfully.
[24bcs056@mepcolinux ex5]$exit
exit
