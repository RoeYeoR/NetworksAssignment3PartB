// RUDP_Sender.c

#include "rudp.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <receiver_ip> <port> <file_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    char *receiver_ip = argv[1];
    int port = atoi(argv[2]);
    char *file_path = argv[3];
    
    // Read the file
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("File open failed");
        exit(EXIT_FAILURE);
    }
    
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *file_buffer = (char *)malloc(file_size);
    if (file_buffer == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    
    fread(file_buffer, 1, file_size, file);
    fclose(file);
    
    // Create RUDP socket
    RUDP_Socket sock;
    sock.sockfd = rudp_socket(port);
    memset(&sock.addr, 0, sizeof(sock.addr));
    sock.addr.sin_family = AF_INET;
    sock.addr.sin_port = htons(port);
    inet_pton(AF_INET, receiver_ip, &sock.addr.sin_addr);
    
    // Send the file via RUDP
    int bytes_sent = rudp_send(&sock, file_buffer, file_size);
    if (bytes_sent != file_size) {
        fprintf(stderr, "Error sending file\n");
        exit(EXIT_FAILURE);
    }
    
    // User decision: Send the file again?
    char choice;
    do {
        printf("Do you want to send the file again? (y/n): ");
        scanf("%c", &choice);
        while (getchar() != '\n'); // Clear input buffer
        
        if (choice == 'y') {
            // Send the file again
            bytes_sent = rudp_send(&sock, file_buffer, file_size);
            if (bytes_sent != file_size) {
                fprintf(stderr, "Error sending file\n");
                exit(EXIT_FAILURE);
            }
        }
    } while (choice == 'y');
    
    // Send exit message to the receiver
    RUDP_Packet exit_packet;
    exit_packet.flags = FIN;
    sendto(sock.sockfd, &exit_packet, sizeof(exit_packet), 0, (struct sockaddr *)&sock.addr, sizeof(sock.addr));
    
    printf("Exit message sent to the receiver\n");
    
    // Close RUDP connection
    rudp_close(&sock);
    
    free(file_buffer);
    
    return 0;
}
