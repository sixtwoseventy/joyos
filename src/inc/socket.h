#ifndef __SOCKET_H_
#define __SOCKET_H_

#include <kern/lock.h>

/* EWWWWWW. So unistd.h defines pause(), which puts the thread
   to sleep until signalled, which is not what we want to happen -
   joyos defines pause(x) as pausing x ms.  So, do some ugly 
   preprocessor stuff so we don't get a conflicting definition for
   pause with unistd.h
*/
#define pause unistd_pause
#include <unistd.h>
#undef pause

#define SOCKET_BUF_SIZE 256

// socket i/o variables 
extern int sockfd;
extern char socket_buffer[SOCKET_BUF_SIZE];
extern struct lock socket_lock;

// TODO replace functionality with bump sensors
int isStuck();

void init_socket(char*);

#endif

