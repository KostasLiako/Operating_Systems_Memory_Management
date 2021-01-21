#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "HPT.h"

//Define variables
#define TRACE_FILE1 "./Trace_files/bzip.trace"
#define TRACE_FILE2 "./Trace_files/gcc.trace"
#define MAX_TRACES 10000
#define OFFSET_BITS 12
#define HASH_SIZE 10

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Input error!\nExample use: %s 'replacement algorithm' maxFrames q 'window_size_of_WS'\nWindow size for WS is optional\n", argv[0]);
        exit(1);
    }

    char *algor = argv[1];
    int maxFrames = atoi(argv[2]);
    int q = atoi(argv[3]);

    int framesPerProcess = maxFrames / 2;
    
    FILE *tf1, *tf2;
    tf1 = fopen(TRACE_FILE1, "r");
    if (tf1 == NULL)
    {
        fprintf(stderr, "Could not open file 1\n");
        exit(1);
    }
    tf2 = fopen(TRACE_FILE2, "r");
    if (tf2 == NULL)
    {
        fprintf(stderr, "Could not open file 2\n");
        exit(1);
    }

    HashPageTable *pt1;
    HashPageTable *pt2;
    queueMemory *queue = initQueue();

    initializeTable(&pt1, HASH_SIZE, algor);
    initializeTable(&pt2, HASH_SIZE, algor);

    int swtch = 0;
    int count1 = 0;
    int count2 = 0;
    unsigned int address;
    char mode;
    while (count1 + count2 < MAX_TRACES)
    {

        if (swtch == 0)
        {
            fscanf(tf1, "%x %c", &address, &mode);

            //printf("1 %d, %c\n", address >> OFFSET_BITS, mode);
            simulation(pt1, pt2, address >> OFFSET_BITS, mode, count1, HASH_SIZE, 1, maxFrames, queue);
            count1++;
            q--;
        }
        else
        {
            fscanf(tf2, "%x %c", &address, &mode);

            // printf("2 %d, %c\n", address >> OFFSET_BITS, mode);
            simulation(pt1, pt2, address >> OFFSET_BITS, mode, count2, HASH_SIZE, 2, maxFrames, queue);
            count2++;
            q--;
        }

        //If q == 0 then we need to read from the other trace file
        if (q == 0)
        {
            swtch = !swtch;
            q = atoi(argv[3]);
        }
    }

    printTable(pt1, HASH_SIZE);
    printf("\n");
    printTable(pt2, HASH_SIZE);

    //printfQueue(queue);

    printStats(pt1, MAX_TRACES, maxFrames);
    printStats(pt2, MAX_TRACES, maxFrames);

    fclose(tf1);
    fclose(tf2);

    return 0;
}