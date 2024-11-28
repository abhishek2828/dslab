#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define MAXLINE 1024

// Function to get CPU utilization from /proc/stat (Linux-specific)
float get_cpu_utilization() {
    FILE *fp;
    char buffer[MAXLINE];
    unsigned long long int user, nice, system, idle;
    unsigned long long int prev_idle, prev_total, total, usage;
    
    prev_idle = prev_total = 0;

    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        perror("Failed to read /proc/stat");
        exit(EXIT_FAILURE);
    }

    // Read the first line from /proc/stat
    fgets(buffer, sizeof(buffer) - 1, fp);
    fclose(fp);

    // Extract CPU times
    sscanf(buffer, "cpu %llu %llu %llu %llu", &user, &nice, &system, &idle);
    
    total = user + nice + system + idle;
    usage = 100.0 * (float)(total - prev_total) / (total - prev_idle);
    
    prev_idle = idle;
    prev_total = total;
    
    return usage;
}

// Function to connect to a server and forward the string
void connect_to_server(const char *server_ip, int port, const char *message, char *response) {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAXLINE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(server_ip);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection to the server failed");
        exit(EXIT_FAILURE);
    }

    // Send message to the server
    send(sockfd, message, strlen(message), 0);

    // Receive response from the server
    bzero(buffer, MAXLINE);
    read(sockfd, buffer, sizeof(buffer));
    strcpy(response, buffer);

    // Close the connection
    close(sockfd);
}

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[MAXLINE];
    char response[MAXLINE];

    // Create socket for load balancer
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address for load balancer
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8888);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) != 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Load balancer is listening on port 8888...\n");

    len = sizeof(cliaddr);

    // Accept client connection
    while (1) {
        connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
        if (connfd < 0) {
            perror("Client accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Client connected.\n");

        // Receive the lowercase string from the client
        bzero(buffer, MAXLINE);
        read(connfd, buffer, sizeof(buffer));
        printf("Received from client: %s\n", buffer);

        // Get CPU utilization of both servers
        float cpu_server1 = get_cpu_utilization();  // Replace with real utilization
        sleep(1);  // Short sleep to avoid rapid CPU checks
        float cpu_server2 = get_cpu_utilization();  // Replace with real utilization

        printf("CPU Load Server 1: %.2f%%, Server 2: %.2f%%\n", cpu_server1, cpu_server2);

        // Choose the server with lower CPU utilization
        if (cpu_server1 < cpu_server2) {
            printf("Routing to Server 1 (port 9998)...\n");
            connect_to_server("127.0.0.1", 9900, buffer, response);
        } else {
            printf("Routing to Server 2 (port 9999)...\n");
            connect_to_server("127.0.0.1", 9901, buffer, response);
        }

        // Send the response back to the client
        send(connfd, response, strlen(response), 0);

        // Close the client connection
        close(connfd);
    }

    close(sockfd);
    return 0;
}
