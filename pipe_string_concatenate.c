#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int main(){
    int f1[2], f2[2];
    char str1[] = "string1";
    char str2[100];
    pid_t p;
    
    if (pipe(f1) == -1){
        fprintf(stderr,"Pipe Failed");
        return 1;
    }

    if(pipe(f2) == -1){
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    printf("Input your string here: ");
    scanf("%s", str2);

    p = fork();
    if(p < 0){
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if(p > 0){
        char concat[100];
        close(f1[0]);
        write(f1[1], str2, strlen(str2) + 1);
        close(f1[1]);

        wait(NULL);

        close(f2[1]);
        read(f2[0], concat, 100);
        printf("Concatenated String: %s", concat);
        close(f2[0]);
    }
    else{
        close(f1[1]);
        char concat[100];
        read(f1[0], concat, 100);
        int k = strlen(concat);
        int n = strlen(str1);
        for(int i = 0; i < k;i++){
            concat[i] -= 32;
        }
        for(int i = 0; i < n; i++){
            concat[k++] = str1[i] - 32;
            printf("str1: %c\n", str1[i]);
            printf("added one word to concat\n");
        }
        concat[k] = '\0';
        close(f1[0]);
        close(f2[0]);
        write(f2[1], concat, strlen(concat) + 1);
        close(f2[1]);

        exit(0);

    }
}