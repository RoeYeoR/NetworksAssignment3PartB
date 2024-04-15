// RUDP_Receiver.c

#include "rudp.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s -p <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int port = atoi(argv[2]);
    
    // Create RUDP socket
    int sockfd = rudp_socket(port);
    
    printf("Starting Receiver...\n");
    printf("Waiting for RUDP connection...\n");
    
    char buffer[MAX_PACKET_SIZE];
    FILE *file = NULL;
    time_t start_time, end_time;
    int total_bytes_received = 0;
    int file_received = 0;
    int num_runs = 0;
    float total_time_ms = 0.0;
    
    while (1) {
        if (!file_received) {
            file = fopen("received_file.txt", "wb");
            if (file == NULL) {
                perror("File open failed");
                exit(EXIT_FAILURE);
            }
            file_received = 1;
        }
        
        start_time = clock();
        
        // Receive data
        int bytes_received = rudp_receive(sockfd, buffer, sizeof(buffer));
        if (bytes_received > 0) {
            fwrite(buffer, 1, bytes_received, file);
            total_bytes_received += bytes_received;
        }
        else {
            break; // No more data to receive
        }
        
        end_time = clock();
        float time_diff_ms = ((float)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
        total_time_ms += time_diff_ms;
        num_runs++;
        
        printf("Received file chunk (%d bytes) in %.2fms\n", bytes_received, time_diff_ms);
        
        // Wait for Sender response
        printf("Waiting for Sender response...\n");
        int exit_message_received = 0;
        while (!exit_message_received) {
            // Receive response from Sender
            int bytes_received = rudp_receive(sockfd, buffer, sizeof(buffer));
            if (bytes_received > 0) {
                if (strcmp(buffer, "exit") == 0) {
                    exit_message_received = 1;
                    break;
                }
                else if (strcmp(buffer, "resend") == 0) {
                    printf("Received resend request from Sender\n");
                    break;
                }
            }
        }
        
        if (exit_message_received) {
            printf("Sender sent exit message.\n");
            break;
        }
    }
    
    fclose(file);
    printf("File transfer completed.\n");
    
    printf("----------------------------------\n");
    printf("- * Statistics * -\n");
    printf("- Total Runs: %d\n", num_runs);
    printf("- Total Bytes Received: %d\n", total_bytes_received);
    printf("- Total Time (ms): %.2f\n", total_time_ms);
    printf("- Average Time per Run (ms): %.2f\n", total_time_ms / num_runs);
    printf("- Average Bandwidth (MB/s): %.2f\n", (float)total_bytes_received / (total_time_ms / 1000) / (1024 * 1024));
    printf("----------------------------------\n");
    
    printf("Receiver end.\n");
    
    // Close RUDP connection
    rudp_close(sockfd);
    
    return 0;
}
