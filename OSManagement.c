// OSManagement
// Created by Stephen Miller on 11/1/21 (Assignment 5).
// CGS 3763, PID 4078565
// Assignment 6 completed 11/19/21
// Enhanced version with improvements - Feb 26, 2025

#include <stdio.h>      // Standard input/output
#include <stdlib.h>     // Standard library (e.g., exit)
#include <string.h>     // String operations
#include <time.h>       // Time-related functions
#include <dirent.h>     // Directory operations
#include <sys/stat.h>   // File status operations
#include <pthread.h>    // POSIX threads
#include <unistd.h>     // POSIX utilities (e.g., sleep)

// Define constants for menu options and algorithm types
#define EXIT 0          // Exit program
#define INVALID -1      // Invalid selection
#define MEMORY 1        // Memory management option
#define FILES 2         // File management option
#define THREAD 3        // Multithreading option
#define FIRST 0         // First Fit algorithm
#define BEST 1          // Best Fit algorithm
#define WORST 2         // Worst Fit algorithm
#define NEXT 3          // Next Fit algorithm
#define MAX_PATH 256    // Maximum path length for file operations

// Structure to track memory block state
typedef struct {
    int originalSize;   // Original size of the memory block
    int remainingSize;  // Remaining available space
    int occupied;       // Flag indicating if block is fully occupied
} MemoryBlock;

// Function prototypes
int displayMenu(void);                  // Displays and handles menu selection
void clearScreen(void);                 // Clears the terminal screen
void memoryManagement(void);            // Manages memory allocation simulations
void fileAttributes(const char *path);  // Displays file attributes from a directory
void *threadFunction(void *arg);        // Thread function for multithreading demo
void displayProcess(int allocation[], int processes, int processSize[], MemoryBlock blocks[], int blocksNum); // Displays allocation results
void initMemoryBlocks(MemoryBlock blocks[], int blockSize[], int blocksNum); // Initializes memory blocks
void firstFit(MemoryBlock blocks[], int blocksNum, int processSize[], int processes); // First Fit algorithm
void worstFit(MemoryBlock blocks[], int blocksNum, int processSize[], int processes); // Worst Fit algorithm
void bestFit(MemoryBlock blocks[], int blocksNum, int processSize[], int processes); // Best Fit algorithm
void nextFit(MemoryBlock blocks[], int blocksNum, int processSize[], int processes); // Next Fit algorithm
void printAttributes(char name[], struct stat statBuff, int showAll); // Prints file attributes

int main(void) {
    int choice = INVALID; // Initialize menu choice to invalid
    while (choice != EXIT) {
        choice = displayMenu(); // Get user selection
        switch (choice) {
            case MEMORY:
                memoryManagement(); // Run memory management simulation
                break;
            case FILES:
                fileAttributes("."); // Run file attributes for current directory
                break;
            case THREAD: {
                pthread_t thread1, thread2; // Declare thread IDs
                printf("\nCreating two sample threads...\n");
                pthread_create(&thread1, NULL, threadFunction, "Thread 1"); // Create first thread
                pthread_create(&thread2, NULL, threadFunction, "Thread 2"); // Create second thread
                pthread_join(thread1, NULL); // Wait for first thread to finish
                pthread_join(thread2, NULL); // Wait for second thread to finish
                clearScreen(); // Clear screen after threads complete
                break;
            }
            case EXIT:
                printf("Exiting program...\n");
                exit(EXIT); // Exit the program
            default:
                printf("Invalid selection\n"); // Handle invalid input
                break;
        }
    }
    return 0; // Return success
}

int displayMenu(void) {
    int choice = INVALID; // Initialize choice to invalid
    char buffer[100];     // Buffer for input
    
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
                choice = INVALID; // Reset to invalid on bad input
            }
        }
    }
    return choice; // Return valid selection
}

void clearScreen(void) {
    printf("\nPress Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Clear input buffer
    system("clear"); // Clear terminal (Mac/Linux)
}

void memoryManagement(void) {
    clearScreen(); // Clear screen before starting
    printf("\n************ Memory Management ************\n");

    int blockSize[] = {15, 10, 20, 35, 80}; // Array of memory block sizes
    int processSize[] = {10, 20, 5, 30, 65}; // Array of process sizes
    int blocksNum = sizeof(blockSize) / sizeof(blockSize[0]); // Number of blocks
    int processes = sizeof(processSize) / sizeof(processSize[0]); // Number of processes
    MemoryBlock blocks[blocksNum]; // Array of MemoryBlock structures

    for (int algorithm = FIRST; algorithm <= NEXT; algorithm++) {
        initMemoryBlocks(blocks, blockSize, blocksNum); // Reset blocks for each algorithm
        
        switch(algorithm) {
            case FIRST:
                firstFit(blocks, blocksNum, processSize, processes); // Run First Fit
                break;
            case BEST:
                bestFit(blocks, blocksNum, processSize, processes); // Run Best Fit
                break;
            case WORST:
                worstFit(blocks, blocksNum, processSize, processes); // Run Worst Fit
                break;
            case NEXT:
                nextFit(blocks, blocksNum, processSize, processes); // Run Next Fit
                break;
        }
    }
}

void initMemoryBlocks(MemoryBlock blocks[], int blockSize[], int blocksNum) {
    for (int i = 0; i < blocksNum; i++) {
        blocks[i].originalSize = blockSize[i];  // Set original size
        blocks[i].remainingSize = blockSize[i]; // Set remaining size
        blocks[i].occupied = 0;                 // Mark as not occupied
    }
}

void firstFit(MemoryBlock blocks[], int blocksNum, int processSize[], int processes) {
    printf("\n**************** First Fit ****************\n");
    int allocation[processes]; // Array to store block allocations
    memset(allocation, INVALID, sizeof(allocation)); // Initialize with -1

    for (int p = 0; p < processes; p++) {
        for (int b = 0; b < blocksNum; b++) {
            if (blocks[b].remainingSize >= processSize[p]) {
                allocation[p] = b;                  // Assign block
                blocks[b].remainingSize -= processSize[p]; // Update remaining space
                break; // Move to next process
            }
        }
    }
    displayProcess(allocation, processes, processSize, blocks, blocksNum); // Display results
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
                    bestIdx = b; // Find smallest fitting block
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
                worstIdx = b; // Find largest fitting block
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
    int lastIdx = 0; // Start index for next fit
    memset(allocation, INVALID, sizeof(allocation));

    for (int p = 0; p < processes; p++) {
        int b = lastIdx;
        int checked = 0;
        while (checked < blocksNum) {
            if (blocks[b].remainingSize >= processSize[p]) {
                allocation[p] = b;
                blocks[b].remainingSize -= processSize[p];
                lastIdx = (b + 1) % blocksNum; // Update last index
                break;
            }
            b = (b + 1) % blocksNum; // Move to next block
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
    clearScreen(); // Clear screen before display
    struct stat statBuff;
    struct dirent *de;
    DIR *dr = opendir(path); // Open directory
    char input[10];
    int showAll = 1;

    if (dr == NULL) {
        printf("Could not open directory: %s\n", path);
        return;
    }

    printf("Show all file details? (y/n): ");
    fgets(input, sizeof(input), stdin);
    if (input[0] == 'n' || input[0] == 'N') {
        showAll = 0; // Toggle detail level
    }

    while ((de = readdir(dr)) != NULL) {
        char fullPath[MAX_PATH];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, de->d_name); // Construct full path
        if (stat(fullPath, &statBuff) == -1) {
            printf("Error reading stats for %s\n", fullPath);
            continue;
        }
        printAttributes(de->d_name, statBuff, showAll); // Print attributes
    }
    closedir(dr); // Close directory
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
    char *threadName = (char *)arg; // Cast argument to thread name
    printf("%s running...\n", threadName);
    sleep(1); // Simulate work for 1 second
    printf("%s completed\n", threadName);
    return NULL; // Thread exit
}