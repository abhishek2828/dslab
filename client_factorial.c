#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock;
    struct sockaddr_in server_addr;
    int num;
    unsigned long long result;

    // Creating socket file descriptor
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Server address setup
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to the server failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Get an integer input from the user
    printf("Enter an integer to calculate its factorial: ");
    scanf("%d", &num);

    // Send the number to the server
    write(sock, &num, sizeof(num));

    // Receive the result from the server
    read(sock, &result, sizeof(result));
    printf("Factorial of %d is %llu\n", num, result);

    // Close the socket
    close(sock);

    return 0;
}
