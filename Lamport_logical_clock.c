#include <stdio.h>
#define NUM_PROCESSES 3

void printVector(int process, int timestamp[NUM_PROCESSES]){
    printf("Process %d Vector Timestamp: [", process + 1);
    for (int i = 0; i < NUM_PROCESSES; i++){
        printf("%d", timestamp[i]);
        if (i < NUM_PROCESSES - 1){
            printf(", ");
        }
    }
    printf("]\n");
}

void simulateEvent(int process, int lamportClock[], int vectorClock[NUM_PROCESSES][NUM_PROCESSES]){
    lamportClock[process]++; //Lamport Clock
    vectorClock[process][process]++; // Vector Clock
    printf("Process %d Lamport Clock: %d\n", process + 1, lamportClock[process]);
    printVector(process, vectorClock[process]);
}

void sendEvent(int sender, int receiver, int lamportClock[], int vectorClock[NUM_PROCESSES][NUM_PROCESSES]){
    lamportClock[sender]++;
    lamportClock[receiver] = (lamportClock[sender] > lamportClock[receiver]) ? lamportClock[sender] + 1: lamportClock[receiver] + 1;

    vectorClock[sender][sender]++;
    for(int i = 0; i < NUM_PROCESSES; i++){
        vectorClock[receiver][i] = (vectorClock[sender][i] > vectorClock[receiver][i]) ? vectorClock[sender][i] : vectorClock[receiver][i];
    }
    printf("Process %d sends event to Process %d\n", sender + 1, receiver + 1);
    printf("Lamport Clock - Sender: %d, Receiver: %d\n", lamportClock[sender], lamportClock[receiver]);
    printVector(sender, vectorClock[sender]);
    printVector(receiver, vectorClock[receiver]);
}

int main(){
    int lamportClock[NUM_PROCESSES] = {0,0,0};
    int vectorClock[NUM_PROCESSES][NUM_PROCESSES] = {{0,0,0}, {0,0,0}, {0,0,0}};

    printf("Combined Lamport's Logical Clock and Vector Clock Synchronization:\n");

    // Simulate events in each process
    simulateEvent(0, lamportClock, vectorClock); // Process 1 event
    simulateEvent(1, lamportClock, vectorClock); // Process 2 event
    simulateEvent(2, lamportClock, vectorClock); // Process 3 event

    // Simulate sending events
    sendEvent(0, 1, lamportClock, vectorClock); // Process 1 sends event to Process 2
    sendEvent(1, 2, lamportClock, vectorClock); // Process 2 sends event to Process 3
    sendEvent(2, 0, lamportClock, vectorClock); // Process 3 sends event to Process 1

    return 0;
}
