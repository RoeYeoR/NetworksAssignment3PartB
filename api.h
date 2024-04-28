#ifndef API_H
#define API_H

#include <stdio.h>

int rudp_socket();
int rudp_send(int sockfd, const void *buf, size_t len, int flags);
int rudp_recv(int sockfd, void *buf, size_t len, int flags);
int rudp_close(int sockfd);

#endif /* API_H */