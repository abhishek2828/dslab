#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>


#define SHARED_MEM_NAME "/shared_mem_example"
#define SEMAPHORE_NAME "/semaphore_example"
#define SHARED_MEM_SIZE sizeof(int)


int main() {
    int shm_fd;
    int *shared_counter;
    sem_t *semaphore;  // Declare semaphore as a pointer to sem_t


    // Create or open the shared memory object
    shm_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }


    // Set the size of the shared memory object
    if (ftruncate(shm_fd, SHARED_MEM_SIZE) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }


    // Map the shared memory object into the process's address space
    shared_counter = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_counter == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }


    // Initialize the shared counter
    *shared_counter = 0;


    // Create or open a named semaphore
    semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 1);
    if (semaphore == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }


    printf("Server Running..........\n");


    while (1) {
        // Wait on the semaphore
        if (sem_wait(semaphore) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }


        // Critical section: increment the shared counter
        (*shared_counter)++;
        printf("Counter has been incremented to: %d\n", *shared_counter);


        // Post the semaphore
        if (sem_post(semaphore) == -1) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }


        // Sleep for 5 seconds
        sleep(5);
    }


    // Cleanup code (this won't be reached in this example, but is important to include)
    if (munmap(shared_counter, SHARED_MEM_SIZE) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }


    if (shm_unlink(SHARED_MEM_NAME) == -1) {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }


    if (sem_close(semaphore) == -1) {
        perror("sem_close");
        exit(EXIT_FAILURE);
    }


    if (sem_unlink(SEMAPHORE_NAME) == -1) {
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }


    return 0;
}
