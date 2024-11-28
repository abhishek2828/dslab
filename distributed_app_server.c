#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void process_request(int client_sock){
    char buffer[BUFFER_SIZE];
    int read_size;

    if((read_size = recv(client_sock, buffer, sizeof(buffer) - 1, 0)) > 0){
        buffer[read_size] = '\0';
        printf("Received from client: %s\n", buffer);

        for(int i = 0; buffer[i]; i++){
            if(buffer[i] >= 'a' && buffer[i] <= 'z'){
                buffer[i] -= 32;
            }
        }

        send(client_sock, buffer, strlen(buffer), 0);
        printf("Processed and sent back: %s\n", buffer);

    }

    close(client_sock);
}

int main(){
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    if((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if(bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if(listen(server_sock, 10) < 0){
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Stateless server listening on port %d...\n", PORT);

    while ((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len))){
        printf("New client connected.\n");

        process_request(client_sock);
    }

    if(client_sock < 0){
        perror("Accept failed");
    }

    close(server_sock);
    return 0;

}