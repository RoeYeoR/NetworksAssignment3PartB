#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "api.h"

#define MAX_BUFFER_SIZE 2048

int main(int argc,char* argv[])
{
    if(argc != 3)
    {
        printf("need 3 arguments..\n");
        return -1;


    }

    int port = atoi(argv[2]);
    int sockfd = rudp_socket();
    if(sockfd <0)
    {
        printf("failed to create a socket..\n");
        return -1;
    }

     // Bind socket to port
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    struct timeval start_time,end_time;
    double total_time = 0.0;
    int file_count=0;
    do {
        gettimeofday(&start_time,NULL);
        char buffer[MAX_BUFFER_SIZE];
        size_t bytes_received;
            
        while(buffer[MAX_BUFFER_SIZE-2] != 'e')

        {
            if(strncmp(buffer,"exit",4)==0)
            {
            
                break;
            }

           if(bytes_received = rudp_recv(sockfd,buffer,MAX_BUFFER_SIZE,port)<0)
           {
                printf("error of receiveing the file...");
                return -1;
           }
        }

        gettimeofday(&end_time,NULL);
        double elapsed_time = (end_time.tv_sec - start_time.tv_sec)*1000 + ((end_time.tv_usec - start_time.tv_usec) /1000);
        total_time += elapsed_time;
        file_count++;
        printf("File number %d received successfully. Time taken: %.2f milliseconds\n",file_count, elapsed_time);

        
        if (rudp_recv(sockfd, &buffer, sizeof(buffer), port) == -1) {
            printf("Failed to receive response from sender\n");
            return -1;
        }

        if (strncmp(buffer, "exit", 4) == 0) {

            printf("Total time taken to receive %d files: %.2f milliseconds\n", file_count, total_time);
            double average_time = total_time / file_count;
            printf("Average time per file: %.2f milliseconds\n", average_time);
            double average_bandwidth = (MAX_BUFFER_SIZE * file_count) / (total_time / 1000.0); // in bytes per second
            printf("Average bandwidth: %.2f bytes/sec\n", average_bandwidth);

        }
            
        
       


    } while(1);

    // Close connection
    if (rudp_close(sockfd) == -1) {
        fprintf(stderr, "Failed to close connection\n");
        exit(EXIT_FAILURE);
    }

    printf("Connection closed\n");
    return 0;


}
