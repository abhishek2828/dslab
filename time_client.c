#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9999
#define MAXLINE 1024

int main() {
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
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection to the server failed");
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server.\n");

    // Receive the message from the server
    read(sockfd, buffer, sizeof(buffer));

    // Print the message received from the server
    printf("Server Response:\n%s\n", buffer);

    // Close the connection
    close(sockfd);

    return 0;
}
