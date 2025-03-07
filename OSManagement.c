// OSManagement
// Created by Stephen Miller on 11/1/21 (Assignment 5).
// CGS 3763, PID 4078565
// Assignment 6 completed 11/19/21
// Enhanced version with improvements - Feb 26, 2025

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>

#define EXIT 0
#define INVALID -1
#define MEMORY 1
#define FILES 2
#define THREAD 3
#define FIRST 0
#define BEST 1
#define WORST 2
#define NEXT 3
#define MAX_PATH 256

typedef struct {
    int originalSize;
    int remainingSize;
    int occupied;
} MemoryBlock;

// Function prototypes
int displayMenu(void);
void clearScreen(void);
void memoryManagement(void);
void fileAttributes(const char *path);
void *threadFunction(void *arg);
void displayProcess(int allocation[], int processes, int processSize[], MemoryBlock blocks[], int blocksNum);
void initMemoryBlocks(MemoryBlock blocks[], int blockSize[], int blocksNum);
void firstFit(MemoryBlock blocks[], int blocksNum, int processSize[], int processes);
void worstFit(MemoryBlock blocks[], int blocksNum, int processSize[], int processes);
void bestFit(MemoryBlock blocks[], int blocksNum, int processSize[], int processes);
void nextFit(MemoryBlock blocks[], int blocksNum, int processSize[], int processes);
void printAttributes(char name[], struct stat statBuff, int showAll);

int main(void) {
    int choice = INVALID;
    while (choice != EXIT) {
        choice = displayMenu();
        switch (choice) {
            case MEMORY:
                memoryManagement();
                break;
            case FILES:
                fileAttributes(".");
                break;
            case THREAD: {
                pthread_t thread1, thread2;
                printf("\nCreating two sample threads...\n");
                pthread_create(&thread1, NULL, threadFunction, "Thread 1");
                pthread_create(&thread2, NULL, threadFunction, "Thread 2");
                pthread_join(thread1, NULL);
                pthread_join(thread2, NULL);
                clearScreen();
                break;
            }
            case EXIT:
                printf("Exiting program...\n");
                exit(EXIT);
            default:
                printf("Invalid selection\n");
                break;
        }
    }
    return 0;
}

int displayMenu(void) {
    int choice = INVALID;
    char buffer[100];
    
    while (choice == INVALID) {
        printf("\nOS Management Simulator\n");
        printf("1. Memory Management\n");
        printf("2. File Management\n");
        printf("3. Multithreading Demo\n");
        printf("0. Exit\n");
        printf("Enter selection: ");
        
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%d", &choice) != 1 || choice < 0 || choice > 3) {
                printf("Please enter a number between 0 and 3\n");
                choice = INVALID;
            }
        }
    }
    return choice;
}

void clearScreen(void) {
    printf("\nPress Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    system("clear");
}

void memoryManagement(void) {
    clearScreen();
    printf("\n************ Memory Management ************\n");

    int blockSize[] = {15, 10, 20, 35, 80};
    int processSize[] = {10, 20, 5, 30, 65};
    int blocksNum = sizeof(blockSize) / sizeof(blockSize[0]);
    int processes = sizeof(processSize) / sizeof(processSize[0]);
    MemoryBlock blocks[blocksNum];

    for (int algorithm = FIRST; algorithm <= NEXT; algorithm++) {
        initMemoryBlocks(blocks, blockSize, blocksNum);
        
        switch(algorithm) {
            case FIRST:
                firstFit(blocks, blocksNum, processSize, processes);
                break;
            case BEST:
                bestFit(blocks, blocksNum, processSize, processes);
                break;
            case WORST:
                worstFit(blocks, blocksNum, processSize, processes);
                break;
            case NEXT:
                nextFit(blocks, blocksNum, processSize, processes);
                break;
        }
    }
}

void initMemoryBlocks(MemoryBlock blocks[], int blockSize[], int blocksNum) {
    for (int i = 0; i < blocksNum; i++) {
        blocks[i].originalSize = blockSize[i];
        blocks[i].remainingSize = blockSize[i];
        blocks[i].occupied = 0;
    }
}

void firstFit(MemoryBlock blocks[], int blocksNum, int processSize[], int processes) {
    printf("\n**************** First Fit ****************\n");
    int allocation[processes];
    memset(allocation, INVALID, sizeof(allocation));

    for (int p = 0; p < processes; p++) {
        for (int b = 0; b < blocksNum; b++) {
            if (blocks[b].remainingSize >= processSize[p]) {
                allocation[p] = b;
                blocks[b].remainingSize -= processSize[p];
                break;
            }
        }
    }
    displayProcess(allocation, processes, processSize, blocks, blocksNum);
}

void bestFit(MemoryBlock blocks[], int blocksNum, int processSize[], int processes) {
    printf("\n**************** Best Fit ****************\n");
    int allocation[processes];
    memset(allocation, INVALID, sizeof(allocation));

    for (int p = 0; p < processes; p++) {
        int bestIdx = INVALID;
        for (int b = 0; b < blocksNum; b++) {
            if (blocks[b].remainingSize >= processSize[p]) {
                if (bestIdx == INVALID || blocks[b].remainingSize < blocks[bestIdx].remainingSize) {
                    bestIdx = b;
                }
            }
        }
        if (bestIdx != INVALID) {
            allocation[p] = bestIdx;
            blocks[bestIdx].remainingSize -= processSize[p];
        }
    }
    displayProcess(allocation, processes, processSize, blocks, blocksNum);
}

void worstFit(MemoryBlock blocks[], int blocksNum, int processSize[], int processes) {
    printf("\n**************** Worst Fit ****************\n");
    int allocation[processes];
    memset(allocation, INVALID, sizeof(allocation));

    for (int p = 0; p < processes; p++) {
        int worstIdx = INVALID;
        int maxSpace = -1;
        for (int b = 0; b < blocksNum; b++) {
            if (blocks[b].remainingSize >= processSize[p] && blocks[b].remainingSize > maxSpace) {
                worstIdx = b;
                maxSpace = blocks[b].remainingSize;
            }
        }
        if (worstIdx != INVALID) {
            allocation[p] = worstIdx;
            blocks[worstIdx].remainingSize -= processSize[p];
        }
    }
    displayProcess(allocation, processes, processSize, blocks, blocksNum);
}

void nextFit(MemoryBlock blocks[], int blocksNum, int processSize[], int processes) {
    printf("\n**************** Next Fit ****************\n");
    int allocation[processes];
    int lastIdx = 0;
    memset(allocation, INVALID, sizeof(allocation));

    for (int p = 0; p < processes; p++) {
        int b = lastIdx;
        int checked = 0;
        while (checked < blocksNum) {
            if (blocks[b].remainingSize >= processSize[p]) {
                allocation[p] = b;
                blocks[b].remainingSize -= processSize[p];
                lastIdx = (b + 1) % blocksNum;
                break;
            }
            b = (b + 1) % blocksNum;
            checked++;
        }
    }
    displayProcess(allocation, processes, processSize, blocks, blocksNum);
}

void displayProcess(int allocation[], int processes, int processSize[], MemoryBlock blocks[], int blocksNum) {
    printf("\nProcess No.\tProcess Size\tBlock No.\tRemaining Space\n");
    for (int i = 0; i < processes; i++) {
        printf("%d\t\t%d\t\t", i + 1, processSize[i]);
        if (allocation[i] == INVALID) {
            printf("Not Allocated\t-\n");
        } else {
            printf("%d\t\t%d\n", allocation[i] + 1, blocks[allocation[i]].remainingSize);
        }
    }
}

void fileAttributes(const char *path) {
    clearScreen();
    struct stat statBuff;
    struct dirent *de;
    DIR *dr = opendir(path);
    char input[10];
    int showAll = 1;

    if (dr == NULL) {
        printf("Could not open directory: %s\n", path);
        return;
    }

    printf("Show all file details? (y/n): ");
    fgets(input, sizeof(input), stdin);
    if (input[0] == 'n' || input[0] == 'N') {
        showAll = 0;
    }

    while ((de = readdir(dr)) != NULL) {
        char fullPath[MAX_PATH];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, de->d_name);
        if (stat(fullPath, &statBuff) == -1) {
            printf("Error reading stats for %s\n", fullPath);
            continue;
        }
        printAttributes(de->d_name, statBuff, showAll);
    }
    closedir(dr);
}

void printAttributes(char name[], struct stat statBuff, int showAll) {
    char timestr[100];
    time_t t;

    printf("\nFile: %s\n", name);
    if (showAll) {
        printf("Size: %lld bytes\n", (long long)statBuff.st_size);
        t = statBuff.st_mtime;
        strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", localtime(&t));
        printf("Last Modified: %s\n", timestr);
        printf("Permissions: %o\n", statBuff.st_mode & 0777);
    }
}

void *threadFunction(void *arg) {
    char *threadName = (char *)arg;
    printf("%s running...\n", threadName);
    sleep(1);
    printf("%s completed\n", threadName);
    return NULL;
}