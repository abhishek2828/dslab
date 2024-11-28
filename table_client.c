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

// Print the received master table
void print_master_table(struct TableEntry *table, int size) {
    printf("Received Updated Master Table from Server:\n");
    printf("%-10s %-16s %-10s\n", "Node No.", "IP Address", "Port No.");
    for (int i = 0; i < size; i++) {
        printf("%-10d %-16s %-10d\n", table[i].node_no, table[i].ip_address, table[i].port_no);
    }
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    struct TableEntry master_table[10]; // Will receive up to 10 entries from the server
    int table_size = 0;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    bzero(&servaddr, sizeof(servaddr));

    // Server address details
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection to the server failed");
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server.\n");

    // Prepare the client table entry
    struct TableEntry client_entry = {4, "172.31.100.60", 8080};

    // Send the client entry to the server
    send(sockfd, &client_entry, sizeof(client_entry), 0);
    printf("Sent to server: Node No: %d, IP: %s, Port: %d\n", client_entry.node_no, client_entry.ip_address, client_entry.port_no);

    // Receive the updated master table from the server
    recv(sockfd, master_table, sizeof(master_table), 0);

    // Print the updated master table
    print_master_table(master_table, 4); // Assuming the table now has 4 rows after update

    // Close the socket
    close(sockfd);

    return 0;
}
