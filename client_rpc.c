#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>

#define PORT 8800
#define BUFFER_SIZE 1024

void receiveFile(int socket, const char *fileName) {
    char buffer[BUFFER_SIZE];
    int bytesReceived;

    // Open file for writing
    int file = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file < 0) {
        perror("File could not be created");
        return;
    }

    // Receive file data from server and write it locally
    while ((bytesReceived = read(socket, buffer, BUFFER_SIZE)) > 0) {
        write(file, buffer, bytesReceived);
    }

    close(file);
    printf("File received and saved as %s\n", fileName);
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection to server failed");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Request file
    char fileName[BUFFER_SIZE];
    printf("Enter file name to request: ");
    scanf("%s", fileName);
    write(clientSocket, fileName, strlen(fileName));

    // Receive and save the file
    receiveFile(clientSocket, fileName);

    close(clientSocket);
    return 0;
}
