#include <stdio.h>
#include <stdbool.h>
#define NUM_PROCESSES 3

int lamportClock[NUM_PROCESSES] = {0,0,0};
bool requestCS[NUM_PROCESSES] = {false, false, false};
bool grantedCS[NUM_PROCESSES] = {false, false, false};
int ricartClock[NUM_PROCESSES] = {0,0,0};

void lamportRequestCS(int process){
    lamportClock[process]++;
    requestCS[process] = true;

    printf("Process %d requests critical section. Lamport clock: %d\n", process + 1, lamportClock[process]);

    for (int i = 0; i < NUM_PROCESSES; i++){
        if (i != process){
            printf("Process %d sends request to Process %d\n", process + 1, i + 1);
            lamportClock[i] = lamportClock[process] + 1;
        }
    }

    for (int i = 0; i < NUM_PROCESSES; i++){
        if (i != process && lamportClock[i] > lamportClock[process]){
            grantedCS[process] = true;
            printf("Process %d received reply from Process %d\n", process + 1, i + 1);
        }
    }

    if (grantedCS[process]){
        printf("Process %d enters critical section (Lamport).\n", process + 1);
    }
}

void lamportReleaseCS(int process) {
    printf("Process %d leaves critical section (Lamport).\n", process + 1);
    requestCS[process] = false;
    grantedCS[process] = false;
}

void ricartRequestCS(int process){
    ricartClock[process]++;
    requestCS[process] = true;

    printf("Process %d requests critical section. Ricart-Agrawala clock: %d\n", process + 1, ricartClock[process]);

    for(int i = 0; i < NUM_PROCESSES; i++){
        if (i != process){
            printf("Process %d sends request to Process %d\n", process + 1, i + 1);
        }
    }

    for (int i = 0; i < NUM_PROCESSES; i++) {
        if (i != process && ricartClock[i] <= ricartClock[process]) {
            grantedCS[process] = true;
            printf("Process %d received permission from Process %d\n", process + 1, i + 1);
        }
    }

    if (grantedCS[process]) {
        printf("Process %d enters critical section (Ricart-Agrawala).\n", process + 1);
    }
}

void ricartReleaseCS(int process) {
    printf("Process %d leaves critical section (Ricart-Agrawala).\n", process + 1);
    requestCS[process] = false;
    grantedCS[process] = false;
}

int main(){
    printf("### Lamport's Algorithm ###\n");
    lamportRequestCS(0); 
    lamportReleaseCS(0); 
    lamportRequestCS(1); 
    lamportReleaseCS(1); 
    lamportRequestCS(2); 
    lamportReleaseCS(2); 

    printf("\n### Ricart-Agrawala Algorithm ###\n");
    ricartRequestCS(0); 
    ricartReleaseCS(0); 
    ricartRequestCS(1); 
    ricartReleaseCS(1); 
    ricartRequestCS(2); 
    ricartReleaseCS(2);

    return 0;
}