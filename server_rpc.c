#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define PORT 8800
#define BUFFER_SIZE 1024

void sendFile(int clientSocket, const char *fileName) {
    int file = open(fileName, O_RDONLY);
    if (file < 0) {
        perror("File not found");
        write(clientSocket, "File not found", 15);
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytesRead;
    while ((bytesRead = read(file, buffer, BUFFER_SIZE)) > 0) {
        write(clientSocket, buffer, bytesRead);
    }

    close(file);
    printf("File sent successfully.\n");
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket to the specified port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(serverSocket, 5) < 0) {
        perror("Listen failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    // Accept and handle incoming connections
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientSocket < 0) {
        perror("Client connection failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Receive the file request
    char fileName[BUFFER_SIZE];
    int bytesReceived = read(clientSocket, fileName, BUFFER_SIZE);
    if (bytesReceived > 0) {
        fileName[bytesReceived] = '\0';
        printf("Request received for file: %s\n", fileName);
        sendFile(clientSocket, fileName);
    }

    close(clientSocket);
    close(serverSocket);
    return 0;
}
