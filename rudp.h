// rudp.h - RUDP header file

#ifndef RUDP_H
#define RUDP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>

#define MAX_PACKET_SIZE 1024
#define TIMEOUT_SEC 1
#define MAX_RETRIES 5

// RUDP packet structure
typedef struct {
    uint16_t length;
    uint16_t checksum;
    uint8_t flags;
    char data[MAX_PACKET_SIZE - 5]; // Adjust according to header size
} RUDP_Packet;

// RUDP socket structure
typedef struct {
    int sockfd;
    struct sockaddr_in addr;
} RUDP_Socket;

// Function prototypes
int rudp_socket(int port);
int rudp_send(RUDP_Socket *sock, const char *data, int length);
int rudp_receive(RUDP_Socket *sock, char *buffer, int buffer_size);
void rudp_close(RUDP_Socket *sock);

#endif
