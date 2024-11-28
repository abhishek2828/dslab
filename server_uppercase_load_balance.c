#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>

#define MAXLINE 1024

// Function to convert lowercase to uppercase
void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int sockfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[MAXLINE];
    socklen_t len;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    // Bind the socket to the port
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) != 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", port);

    len = sizeof(cliaddr);

    // Accept client connection
    while (1) {
        connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
        if (connfd < 0) {
            perror("Server accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Client connected.\n");

        // Receive the string from the client
        bzero(buffer, MAXLINE);
        read(connfd, buffer, sizeof(buffer));
        printf("Received: %s\n", buffer);

        // Convert to uppercase
        to_uppercase(buffer);

        // Send back the uppercase string
        send(connfd, buffer, strlen(buffer), 0);

        // Close the connection
        close(connfd);
    }

    close(sockfd);
    return 0;
}
