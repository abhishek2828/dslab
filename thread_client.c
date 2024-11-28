#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


#define PORT 8000

int main(){
    int sock;
    struct sockaddr_in server;
    char message[1024], server_reply[1024];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1){
        perror("Could not create a socket");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0){
        perror("Connection Failed");
        return 1;
    }

    printf("Connected to the server.\n");

    while(1){
        printf("Enter Message: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message,"\n")] = '\0';

        if (strcmp(message, "exit") == 0){
            break;
        }

        if(send(sock, message, strlen(message), 0) < 0){
            perror("Send Failed");
            return 1;
        }

        if(recv(sock, server_reply, sizeof(server_reply), 0) < 0){
            perror("Recv Failed");
            return 1;
        }

        printf("Server: %s\n", server_reply);
    }

    close(sock);
    return 0;
}