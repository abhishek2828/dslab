#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9999
#define MAXLINE 1024

// Structure to store table entries
struct TableEntry {
    int node_no;
    char ip_address[16];
    int port_no;
};

// Print the current master table
void print_master_table(struct TableEntry *table, int size) {
    printf("Updated Master Table:\n");
    printf("%-10s %-16s %-10s\n", "Node No.", "IP Address", "Port No.");
    for (int i = 0; i < size; i++) {
        printf("%-10d %-16s %-10d\n", table[i].node_no, table[i].ip_address, table[i].port_no);
    }
}

int main() {
    int sockfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    struct TableEntry master_table[10]; // Maximum 10 rows in the table
    int table_size = 3;

    // Initialize master table
    master_table[0] = (struct TableEntry){1, "172.31.100.36", 2345};
    master_table[1] = (struct TableEntry){2, "172.31.100.40", 3128};
    master_table[2] = (struct TableEntry){3, "172.31.100.52", 2323};

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    bzero(&servaddr, sizeof(servaddr));

    // Bind the socket to the port
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if ((listen(sockfd, 5)) != 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening...\n");

    len = sizeof(cliaddr);

    // Accept the client connection
    connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
    if (connfd < 0) {
        perror("Server accept failed");
        exit(EXIT_FAILURE);
    }
    printf("Client connected.\n");

    // Receive client data
    struct TableEntry client_entry;
    recv(connfd, &client_entry, sizeof(client_entry), 0);
    printf("Received from client: Node No: %d, IP: %s, Port: %d\n", client_entry.node_no, client_entry.ip_address, client_entry.port_no);

    // Update master table with the new entry
    master_table[table_size] = client_entry;
    table_size++;

    // Send updated master table back to client
    send(connfd, master_table, sizeof(master_table), 0);

    // Print the updated master table
    print_master_table(master_table, table_size);

    // Close the socket
    close(connfd);
    close(sockfd);

    return 0;
}
