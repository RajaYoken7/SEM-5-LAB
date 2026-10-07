[24bcs057@mepcolinux ex8]$cat paging.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PAGES 4096
#define INPUT_BUF 1024

static unsigned long rngState = 12345UL;

static unsigned long nextRand(void)
{
    rngState = (rngState * 1103515245UL + 12345UL) & 0x7FFFFFFFUL;
    return rngState;
}

int main(void)
{
    long long processSize, pageSize;
    int numPages, totalFrames, i, j, tmp;
    int pageTable[MAX_PAGES];
    int frames[MAX_PAGES];
    char line[INPUT_BUF];
    char *p;

    printf("PAGING : LOGICAL ADDRESS TO PHYSICAL ADDRESS\n\n");

    printf("Enter process size (bytes) : ");
    if (scanf("%lld", &processSize) != 1 || processSize <= 0)
    {
        printf("Invalid process size.\n");
        return 1;
    }

    printf("Enter page size (bytes)    : ");
    if (scanf("%lld", &pageSize) != 1 || pageSize <= 0)
    {
        printf("Invalid page size.\n");
        return 1;
    }

    numPages = (int)((processSize + pageSize - 1) / pageSize);

    if (numPages > MAX_PAGES)
    {
        printf("Too many pages required (%d), max is %d.\n", numPages, MAX_PAGES);
        return 1;
    }

    printf("\nProcess size : %lld bytes\n", processSize);
    printf("Page size    : %lld bytes\n", pageSize);
    printf("Pages needed : %d\n", numPages);

    if (processSize % pageSize != 0)
        printf("Fragmentation: %lld bytes wasted in last page\n",
               pageSize - (processSize % pageSize));

    totalFrames = numPages;
    printf("Frames       : %d\n", totalFrames);

    for (i = 0; i < totalFrames; i++)
        frames[i] = i;

    rngState = 12345UL;
    for (i = totalFrames - 1; i > 0; i--)
    {
        j = (int)(nextRand() % (unsigned long)(i + 1));
        tmp = frames[i];
        frames[i] = frames[j];
        frames[j] = tmp;
    }

    for (i = 0; i < numPages; i++)
        pageTable[i] = frames[i];

    printf("\nPage Table\n");
    printf("Page  Frame\n");
    for (i = 0; i < numPages; i++)
        printf(" %3d   %4d\n", i, pageTable[i]);

    printf("\nEnter logical addresses (comma or space separated, -1 to exit)\n");

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    while (1)
    {
        printf("\n> ");
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        p = line;
        while (*p)
        {
            while (*p && !isdigit((unsigned char)*p) && *p != '-')
                p++;
            if (!*p)
                break;

            long long logicalAddress = strtoll(p, &p, 10);

            if (logicalAddress < 0)
            {
                printf("Exiting.\n");
                return 0;
            }

            if (logicalAddress >= processSize)
            {
                printf("Invalid address %lld. Valid range: 0 to %lld\n",
                       logicalAddress, processSize - 1);
                continue;
            }

            long long page   = logicalAddress / pageSize;
            long long offset = logicalAddress % pageSize;
            int frame        = pageTable[page];
            long long physicalAddress = (long long)frame * pageSize + offset;

            printf("Logical %4lld   Page %3lld   Offset %3lld   Frame %3d   Physical %4lld\n",
                   logicalAddress, page, offset, frame, physicalAddress);
        }
    }

    return 0;
}



[24bcs057@mepcolinux ex8]$./paging
PAGING : LOGICAL ADDRESS TO PHYSICAL ADDRESS

Enter process size (bytes) : 1000
Enter page size (bytes)    : 100

Process size : 1000 bytes
Page size    : 100 bytes
Pages needed : 10
Frames       : 10

Page Table
Page  Frame
   0      9
   1      8
   2      3
   3      0
   4      7
   5      2
   6      1
   7      4
   8      5
   9      6

Enter logical addresses (comma or space separated, -1 to exit)

> 0
Logical    0   Page   0   Offset   0   Frame   9   Physical  900

> 150
Logical  150   Page   1   Offset  50   Frame   8   Physical  850

> 999
Logical  999   Page   9   Offset  99   Frame   6   Physical  699

> -1
Exiting.


  
