#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8001

float cpu_load(){
    FILE* fp = fopen("/proc/stat", "r");
    if (!fp){
        perror("fopen");
        return -1;
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), fp);
    fclose(fp);

    long user, nice, system, idle;
    sscanf(buffer, "cpu %ld %ld %ld %ld", &user, &nice, &system, &idle);
    long total = user + nice + system + idle;
    long busy = total - idle;
    return (float)busy / total * 100;
}

int main(){
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Socket Failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d.....\n", PORT);

    while(1){
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
            perror("accept");
            exit(EXIT_FAILURE);
        }

        float load = cpu_load();

        snprintf(buffer, sizeof(buffer), "CPU Load: %.2f%%\n", load);
        send(new_socket, buffer, strlen(buffer), 0);

        close(new_socket);
    }
    return 0;
}   
