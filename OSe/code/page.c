#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <unistd.h>

#define MAX_FRAMES 3
#define MAX_PAGES 8
#define MAX_STRING 20

int frames[MAX_FRAMES];
int pageReference[MAX_STRING];

//initialize
void initializeFrames() {
    for (int i = 0; i < MAX_FRAMES; i++) {
        frames[i] = -1;
    }
}

void printFrames() {
    for (int i = 0; i < MAX_FRAMES; i++) {
        if (frames[i] == -1) {
            printf("X ");
        } else {
            printf("%d ", frames[i]);
        }
    }
    printf("\n");
}

int isPageInFrames(int page) {
    for (int i = 0; i < MAX_FRAMES; i++) {
        if (frames[i] == page) {
            return i;
        }
    }
    return -1;
}

void FIFO() {
    int pageFaults = 0;
    int currentIndex = 0;

    for (int i = 0; i < MAX_STRING; i++) {
        int page = pageReference[i];
        printf("Page %d: ", page);

        if (isPageInFrames(page)==-1) {
            frames[currentIndex] = page;
            currentIndex = (currentIndex + 1) % MAX_FRAMES;
            pageFaults++;
        }
        printFrames();
    }

    printf("FIFO Page Faults: %d\n", pageFaults);
    printf("FIFO Page Fault Rate: %.2f%%\n", (float)pageFaults / MAX_STRING * 100);
}

void LRU() {
    int pageFaults = MAX_FRAMES;
    int Counter[MAX_FRAMES] = {0};
	
	for(int i = 0; i < MAX_FRAMES; i++) {
		Counter[i]=MAX_FRAMES-i;
		frames[i]=pageReference[i];
		printf("Page %d: ",frames[i]);
		printFrames();
	}
    for (int i = MAX_FRAMES; i < MAX_STRING; i++) {
        int page = pageReference[i];
        printf("Page %d: ",page);

        if (isPageInFrames(page)==-1) {
            int replaceIndex = 0;
            
            for (int j = 1; j < MAX_FRAMES; j++) {
                if (Counter[j] > Counter[replaceIndex]) {
                    replaceIndex = j;
                }
            }

            frames[replaceIndex] = page;
            pageFaults++;
        }

        Counter[isPageInFrames(page)] = 0;
        for (int j = 0; j < MAX_FRAMES; j++) {
            if (frames[j] != -1) {
                Counter[j]++;
            }
        }
        
        printFrames();
    }

    printf("LRU Page Faults: %d\n", pageFaults);
    printf("LRU Page Fault Rate: %.2f%%\n", (float)pageFaults / MAX_STRING * 100);
}


int main() {
    initializeFrames();
    int choice, i;

    //select algorithm
    printf("Select the page replacement algorithm:\n");
    printf("1. FIFO\n");
    printf("2. LRU\n");
    scanf("%d", &choice);
	//invalid input
    if (choice != 1 && choice != 2) {
        printf("Invalid choice.\n");
        return 1;
    }

    //select way
    printf("Select how to generate page references:\n");
    printf("1. Random\n");
    printf("2. Manual Input\n");
    scanf("%d", &i);

    if (i != 1 && i != 2) {
        printf("Invalid choice.\n");
        return 1;
    }

    if (i == 1) {
        srand(time(NULL));
        printf("Generated Page References: ");
        for (i = 0; i < MAX_STRING; i++) {
            pageReference[i] = rand() % MAX_PAGES;
            printf("%d ", pageReference[i]);
        }
        printf("\n");
    } else {
        printf("Enter Page References (separate with spaces): ");
        for (i = 0; i < MAX_STRING; i++) {
            scanf("%d", &pageReference[i]);
        }
    }

    if (choice == 1) {
        printf("\nFIFO Algorithm:\n");
        FIFO();
    } else {
        printf("\nLRU Algorithm:\n");
        LRU();
    }

    return 0;
}
