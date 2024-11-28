#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>

#define SHARED_MEMORY_NAME "/my_shared_memory"
#define SEMAPHORE_NAME "/my_semaphore"

int main() {
    int fd;
    int *counter;
    sem_t *sem;

    fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (counter == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    sem = sem_open(SEMAPHORE_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    if (sem_wait(sem) == -1) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    printf("Counter value: %d\n", *counter);

    if (sem_post(sem) == -1) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }

    if (munmap(counter, sizeof(int)) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    if (sem_close(sem) == -1) {
        perror("sem_close");
        exit(EXIT_FAILURE);
    }

    return 0;
}
