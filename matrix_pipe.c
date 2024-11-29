#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
 
#define ARRAY_SIZE 2
 
int main() {
    int fd1[2]; 
    int fd2[2];
 
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
 
    pid_t p = fork();
 
    if (p < 0) {
        perror("fork");
        exit(-1);
    }
 
    if (p > 0) {
 
        int temparr[ARRAY_SIZE][ARRAY_SIZE];
        int arr[ARRAY_SIZE][ARRAY_SIZE] = {{3,6},{1,4}};
 
 
        close(fd1[0]);
        close(fd2[1]);
 
 
        write(fd1[1], arr, sizeof(arr));
        close(fd1[1]);
 
 
        read(fd2[0], temparr, sizeof(temparr));
        close(fd2[0]);
 
        printf("Final print\n");
        for (int i = 0; i < ARRAY_SIZE; i++) {
        	for (int j = 0; j<ARRAY_SIZE;j++){
            		printf("%d ", temparr[i][j]);
            	}
            	printf("\n");
        }
 
    } else {
 
        int temparr[ARRAY_SIZE][ARRAY_SIZE];
        int arr[ARRAY_SIZE][ARRAY_SIZE] = {{1,2},{3,4}};
 
 
        close(fd1[1]);
        close(fd2[0]);
 
 
        read(fd1[0], temparr, sizeof(temparr));
        close(fd1[0]);
 
        printf("Before adding\n");
        for (int i = 0; i < ARRAY_SIZE; i++) {
        	for (int j = 0; j<ARRAY_SIZE;j++){
            		printf("%d ", temparr[i][j]);
            	}
            	printf("\n");
        }
 
 
        for (int i = 0; i < ARRAY_SIZE; i++) {
        	for (int j = 0; j<ARRAY_SIZE;j++){
            		temparr[i][j] = temparr[i][j] + arr[i][j]; 
            }
            printf("\n");
        }
 
        
 
 
        write(fd2[1], temparr, sizeof(temparr));
        close(fd2[1]); 
        exit(0);
    }
 
    return 0;
}
