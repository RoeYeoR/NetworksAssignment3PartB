
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include "api.h"

#define MAX_BUFFER_SIZE 2048
#define TIMEOUT_SEC 1

// Struct for RUDP header
struct rudp_header {
    uint16_t checksum;
    uint8_t flags;
};

// Flags for RUDP packets
#define RUDP_SYN 0x01
#define RUDP_ACK 0x02
#define RUDP_FIN 0x04

// Function to calculate checksum
unsigned short int calculate_checksum(void *data, unsigned int bytes) { 
    unsigned short int *data_pointer = (unsigned short int *)data; 
    unsigned int total_sum = 0; 
 
    // Main summing loop 
    while (bytes > 1) { 
        total_sum += *data_pointer++; 
        bytes -= 2; 
    } 
 
    // Add left-over byte, if any 
    if (bytes > 0) 
        total_sum += *((unsigned char *)data_pointer); 
 
    // Fold 32-bit sum to 16 bits 
    while (total_sum >> 16) 
        total_sum = (total_sum & 0xFFFF) + (total_sum >> 16); 
 
    return (~((unsigned short int)total_sum)); 
}

// Function to create RUDP socket and perform handshake
int rudp_socket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

// Function to send data using RUDP
int rudp_send(int sockfd, const void *buf, size_t len, int flags) {
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(flags); // Assuming flags parameter contains port
    dest_addr.sin_addr.s_addr = INADDR_ANY; // Not used in sender

    struct rudp_header header;
    header.checksum = calculate_checksum(buf, len);
    header.flags = RUDP_SYN; // Assuming it's always SYN for simplicity

    ssize_t bytes_sent = sendto(sockfd, &header, sizeof(header), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (bytes_sent == -1) {
        perror("Send failed");
        return -1;
    }

    printf("Sent SYN packet.\n");

    // Wait for ACK
    struct sockaddr_in src_addr;
    socklen_t addrlen = sizeof(src_addr);
    struct rudp_header recv_header;
    ssize_t bytes_received = recvfrom(sockfd, &recv_header, sizeof(recv_header), 0, (struct sockaddr *)&src_addr, &addrlen);
    if (bytes_received == -1) {
        perror("Receive failed");
        return -1;
    }

    if (recv_header.flags == RUDP_ACK) {
        printf("Received ACK for SYN packet.\n");
    } else {
        printf("Unexpected packet received.\n");
    }

    return 0;
}

// Function to receive data using RUDP
int rudp_recv(int sockfd, void *buf, size_t len, int flags) {
    struct sockaddr_in src_addr;
    socklen_t addrlen = sizeof(src_addr);
    struct rudp_header header;

    ssize_t bytes_received = recvfrom(sockfd, &header, sizeof(header), 0, (struct sockaddr *)&src_addr, &addrlen);
    if (bytes_received == -1) {
        perror("Receive failed");
        return -1;
    }

    if (header.flags == RUDP_SYN) {
        printf("Received SYN packet.\n");

        // Send ACK
        header.flags = RUDP_ACK;
        ssize_t bytes_sent = sendto(sockfd, &header, sizeof(header), 0, (struct sockaddr *)&src_addr, addrlen);
        if (bytes_sent == -1) {
            perror("Send failed");
            return -1;
        }
        printf("Sent ACK for SYN packet.\n");
    }

    return 0;
}

// Function to close connection
int rudp_close(int sockfd) {
    close(sockfd);
    return 0;
}
