#ifndef __SOCKET_H_
#define __SOCKET_H_

#include <kern/lock.h>

#define SOCKET_BUF_SIZE 256

// socket i/o variables 
extern int sockfd;
extern char socket_buffer[SOCKET_BUF_SIZE];
extern struct lock socket_lock;

// TODO replace functionality with bump sensors
int isStuck();

void init_socket();

#endif

