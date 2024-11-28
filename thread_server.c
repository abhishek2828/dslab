#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8000
#define MAX_CLIENTS 100

void *handle_client(void* socket_desc){
    int client_sock = *(int*)socket_desc;
    char buffer[1024];
    int read_size;

    while ((read_size = recv(client_sock, buffer, sizeof(buffer), 0)) > 0){
        buffer[read_size] = '\0';
        printf("Client: %s\n", buffer);
        send(client_sock, buffer, strlen(buffer), 0);

    }

    if (read_size == 0){
        printf("Client Disconnected.\n");
    }
    else if(read_size == -1){
        perror("recv failed");
    }

    close(client_sock);
    free(socket_desc);
    return NULL;
}

int main(){
    int server_sock, client_sock, *new_sock;
    struct sockaddr_in server, client;
    socklen_t client_size = sizeof(client);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock == -1){
        perror("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server, sizeof(server)) < 0){
        perror("Bind Failed");
        return 1;
    }

    listen(server_sock, MAX_CLIENTS);
    printf("Server is listening on port %d...\n", PORT);
    while((client_sock = accept(server_sock, (struct sockaddr *)&client, &client_size))){
        printf("Client connected.\n");

        pthread_t client_thread;
        new_sock = malloc(sizeof(int));
        *new_sock = client_sock;

        if(pthread_create(&client_thread, NULL, handle_client, (void *)new_sock) < 0){
            perror("Could not create thread");
            return 1;
        }
    }

    if(client_sock < 0){
        perror("Accept failed");
        return 1;
    }

    return 0;

}