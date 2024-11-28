#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 9999
#define MAXLINE 1024

// Function to get the current CPU load from /proc/stat (Linux-specific)
float get_cpu_load() {
    long double a[4], b[4], load;
    FILE *fp;

    // First reading of CPU statistics
    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &a[0], &a[1], &a[2], &a[3]);
    fclose(fp);

    sleep(1);  // Sleep for a second to get the second reading

    // Second reading of CPU statistics
    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &b[0], &b[1], &b[2], &b[3]);
    fclose(fp);

    // Calculate the CPU load percentage
    load = ((b[0] + b[1] + b[2]) - (a[0] + a[1] + a[2])) /
           ((b[0] + b[1] + b[2] + b[3]) - (a[0] + a[1] + a[2] + a[3]));

    return load * 100;
}

// Function to get the current date and time as a string
void get_current_datetime(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[MAXLINE];
    char datetime[64];
    float cpu_load;
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
    servaddr.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) != 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", PORT);

    len = sizeof(cliaddr);

    // Accept client connection
    connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
    if (connfd < 0) {
        perror("Server accept failed");
        exit(EXIT_FAILURE);
    }
    printf("Client connected.\n");

    // Get the current date and time
    get_current_datetime(datetime, sizeof(datetime));

    // Get the current CPU load
    cpu_load = get_cpu_load();

    // Create the response message
    snprintf(buffer, sizeof(buffer), "Date and Time: %s\nCPU Load: %.2f%%\n", datetime, cpu_load);

    // Send the message to the client
    send(connfd, buffer, strlen(buffer), 0);

    // Close the connection
    close(connfd);
    close(sockfd);

    return 0;
}
