#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(){
    int sock;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE], server_reply[BUFFER_SIZE];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    while(1){
        printf("Enter a message: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';

        if(strcmp(message, "exit") == 0){
            break;
        }

        if (send(sock, message, strlen(message), 0)< 0){
            perror("Send Failed");
            break;
        }

        int read_size = recv(sock, server_reply, sizeof(server_reply) - 1, 0);
        if(read_size > 0){
            server_reply[read_size] = '\0';
            printf("Server reply: %s\n", server_reply);
        }
        else{
            perror("Receive failed");
            break;
        }
    }
    close(sock);
    return 0;
}