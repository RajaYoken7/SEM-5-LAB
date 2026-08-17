#include <stdio.h>
#include <string.h>

int main() {
    char data[50];
    int datalen, r, codelen, i, j, k, pos;
    int codeword[50], received[50];
    int choice, errorpos;

    printf("Input binary data stream: ");
    scanf("%s", data);

    datalen = strlen(data);

    r = 0;
    while ((1 << r) < datalen + r + 1) {
        r = r + 1;
    }

    codelen = datalen + r;

    j = 0;
    for (i = 1; i <= codelen; i++) {
        if ((i & (i - 1)) == 0) {
            codeword[i] = -1;
        } else {
            codeword[i] = data[j] - '0';
            j = j + 1;
        }
    }

    for (i = 0; i < r; i++) {
        pos = 1 << i;
        int count = 0;
        for (j = pos; j <= codelen; j = j + 1) {
            if (((j / pos) % 2) == 1) {
                if (codeword[j] == 1) {
                    count = count + 1;
                }
            }
        }
        if (count % 2 == 0) {
            codeword[pos] = 0;
        } else {
            codeword[pos] = 1;
        }
    }

    printf("\n*** TRANSMISSION SOURCE ***\n");
    printf("Original bits: %s\n", data);
    printf("Redundant bits calculated: %d\n", r);
    printf("Generated Hamming code: ");
    for (i = 1; i <= codelen; i++) {
        printf("%d", codeword[i]);
    }
    printf("\n");

    for (i = 1; i <= codelen; i++) {
        received[i] = codeword[i];
    }

    printf("\nSimulate a transmission error? (Press 1 for Yes, 0 for No): ");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("Specify the index to alter (Range 1 to %d): ", codelen);
        scanf("%d", &errorpos);
        if (received[errorpos] == 0) {
            received[errorpos] = 1;
        } else {
            received[errorpos] = 0;
        }
    }

    printf("\n*** DESTINATION RECEIVER ***\n");
    printf("Incoming stream: ");
    for (i = 1; i <= codelen; i++) {
        printf("%d", received[i]);
    }
    printf("\n");

    int syndrome = 0;
    for (i = 0; i < r; i++) {
        pos = 1 << i;
        int count = 0;
        for (j = pos; j <= codelen; j = j + 1) {
            if (((j / pos) % 2) == 1) {
                if (received[j] == 1) {
                    count = count + 1;
                }
            }
        }
        if (count % 2 != 0) {
            syndrome = syndrome + pos;
        }
    }

    printf("Calculated check value: %d\n", syndrome);

    if (syndrome == 0) {
        printf("Status: Stream is error-free\n");
    } else {
        printf("Status: Corrupted bit detected at index %d\n", syndrome);
        if (received[syndrome] == 0) {
            received[syndrome] = 1;
        } else {
            received[syndrome] = 0;
        }
        printf("Fixed sequence: ");
        for (i = 1; i <= codelen; i++) {
            printf("%d", received[i]);
        }
        printf("\n");
    }

    j = 0;
    char correcteddata[50];
    for (i = 1; i <= codelen; i++) {
        if ((i & (i - 1)) != 0) {
            correcteddata[j] = received[i] + '0';
            j = j + 1;
        }
    }
    correcteddata[j] = '\0';

    printf("Final recovered data: %s\n", correcteddata);

    return 0;
}
