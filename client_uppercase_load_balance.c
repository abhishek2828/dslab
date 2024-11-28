#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXLINE 1024

int main() {
    int sockfd;
    struct sockaddr_in lbaddr;
    char message[MAXLINE];
    char buffer[MAXLINE];

    // Create socket for client
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize load balancer address
    bzero(&lbaddr, sizeof(lbaddr));
    lbaddr.sin_family = AF_INET;
    lbaddr.sin_port = htons(8888);
    lbaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the load balancer
    if (connect(sockfd, (struct sockaddr *)&lbaddr, sizeof(lbaddr)) != 0) {
        perror("Connection to the load balancer failed");
        exit(EXIT_FAILURE);
    }

    // Get input from the user
    printf("Enter a lowercase string: ");
    fgets(message, MAXLINE, stdin);
    message[strcspn(message, "\n")] = 0;  // Remove trailing newline

    // Send the string to the load balancer
    send(sockfd, message, strlen(message), 0);

    // Receive the response from the load balancer
    bzero(buffer, MAXLINE);
    read(sockfd, buffer, sizeof(buffer));
    printf("Uppercase string from server: %s\n", buffer);

    // Close the connection
    close(sockfd);
    return 0;
}
