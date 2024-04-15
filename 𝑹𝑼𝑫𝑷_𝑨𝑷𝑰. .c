// RUDP_API.c

#include "rudp.h"

int rudp_socket(int port) {
    int sockfd;
    struct sockaddr_in addr;
    
    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Bind socket to port
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Perform handshake with receiver
    // Send SYN packet
    RUDP_Packet syn_packet;
    syn_packet.flags = SYN;
    sendto(sockfd, &syn_packet, sizeof(syn_packet), 0, (struct sockaddr *)&addr, sizeof(addr));
    
    // Receive ACK packet
    RUDP_Packet ack_packet;
    recvfrom(sockfd, &ack_packet, sizeof(ack_packet), 0, NULL, NULL);
    if (ack_packet.flags != ACK) {
        fprintf(stderr, "Handshake failed\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Connection established with sender\n");
    
    return sockfd;
}

int rudp_send(RUDP_Socket *sock, const char *data, int length) {
    // Construct RUDP packet
    RUDP_Packet packet;
    packet.length = length;
    packet.checksum = calculate_checksum(data, length);
    packet.flags = 0; // Adjust flags as needed
    memcpy(packet.data, data, length);
    
    // Send packet
    sendto(sock->sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&sock->addr, sizeof(sock->addr));
    
    // Wait for acknowledgment
    RUDP_Packet ack_packet;
    int retries = 0;
    while (1) {
        if (recvfrom(sock->sockfd, &ack_packet, sizeof(ack_packet), 0, NULL, NULL) != -1) {
            if (ack_packet.flags == ACK) {
                printf("Packet sent successfully\n");
                return 1;
            }
        }
        else {
            // Retransmit packet if no acknowledgment received
            if (retries < MAX_RETRIES) {
                printf("Retransmitting packet\n");
                sendto(sock->sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&sock->addr, sizeof(sock->addr));
                retries++;
            }
            else {
                fprintf(stderr, "Maximum retries reached\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

int rudp_receive(RUDP_Socket *sock, char *buffer, int buffer_size) {
    // Receive packet
    RUDP_Packet packet;
    recvfrom(sock->sockfd, &packet, sizeof(packet), 0, NULL, NULL);
    
    // Verify checksum
    if (packet.checksum != calculate_checksum(packet.data, packet.length)) {
        printf("Checksum error\n");
        return 0;
    }
    
    // Send acknowledgment
    RUDP_Packet ack_packet;
    ack_packet.flags = ACK;
    sendto(sock->sockfd, &ack_packet, sizeof(ack_packet), 0, (struct sockaddr *)&sock->addr, sizeof(sock->addr));
    
    // Copy received data to buffer
    memcpy(buffer, packet.data, packet.length);
    
    return packet.length;
}

void rudp_close(RUDP_Socket *sock) {
    close(sock->sockfd);
}
