#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "api.h"

#define MAX_BUFFER_SIZE 2048 
#define FILE_SIZE 2000000


char* generate_random_data(unsigned int size) { 
    char *buffer = NULL; 
 
    // Argument check. 
    if (size == 0) 
        return NULL; 
 
    buffer = (char *)calloc(size, sizeof(char)); 
 
    // Error checking. 
    if (buffer == NULL) 
        return NULL; 
 
    // Randomize the seed of the random number generator. 
    srand(time(NULL)); 
 
    for (unsigned int i = 0; i < size-3; i++) 
        *(buffer + i) = ((unsigned int)rand() % 256); 
    
    *(buffer+size-1) = 'e';

    return buffer; 
} 

int main(int argc, char* argv[])
{
    char* fptr = NULL;

    if(argc != 5)
    {
        printf("need 5 arguments ...\n");
        return -1;
    }

    char* receiver_ip ;
    int port;

    for (int i = 1; i < argc; i+=2 )
    {
        if(strcmp(argv[i],"-ip")==0)
        {
            receiver_ip = argv[i+1];
        }
        else if(strcmp(argv[i],"-p")==0)
        {
            port = atoi(argv[i+1]);
        }
    }

    fptr = generate_random_data(FILE_SIZE);
    

    if(fptr ==NULL)
    {
        printf("error of generate a file...\n");
        return -1;
    }

   

    int sockfd = rudp_socket();

    if(sockfd<0)
    {
        perror("failed to create a socket..\n");
        return -1;
    }
    if(rudp_send(sockfd,NULL,0,port)==-1)
        {
            printf("failed to connect to the receiver..\n");
            return -1;
        }

    char decision = 'y';

    while(decision =='y')
    {
        

    size_t total_size = strlen(fptr);
    char buffer[MAX_BUFFER_SIZE];
    size_t bytes_sent = 0;

    if (rudp_send(sockfd, fptr, strlen(fptr), port) == -1)

    while(bytes_sent < total_size)
    {
        size_t remaining_bytes = total_size - bytes_sent;
        size_t current_buffer_size = (remaining_bytes > MAX_BUFFER_SIZE) ? MAX_BUFFER_SIZE : remaining_bytes;

        if (rudp_send(sockfd, fptr + bytes_sent, current_buffer_size, port) == -1)
        {
            printf("error sending the data..\n");
            return -1;
        }


        bytes_sent += current_buffer_size;
    }

    printf("File sent succesfully !\n");

    printf("Do you want to send the file again? (y/n)\n");
    scanf("%c",&decision);
   

    }

    char exit_message[] = "exit";
     if(rudp_send(sockfd,exit_message,strlen(exit_message),port)==-1)
     {
        printf("failed to send exit message..\n");
        return -1;
     }

     if(rudp_close(sockfd)==-1)
     {
        printf("failed to close the connection..\n");
        return -1;
     }

    printf("connection closed...\n");


    return 0;
}
