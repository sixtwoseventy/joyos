#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <kern/lock.h>
#include <kern/global.h>
#include <socket.h>

// socket i/o variables 
int sockfd;
char socket_buffer[SOCKET_BUF_SIZE];
struct lock socket_lock;

// TODO replace functionality with bump sensors
int isStuck(){

	int result;

	acquire(&socket_lock);
	write(sockfd, "s\n", 2);
	read(sockfd, socket_buffer, SOCKET_BUF_SIZE);
    result = (int)(socket_buffer[0] == 't');
	release(&socket_lock);
	
  	return result;
}

void init_socket(){

    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

	init_lock(&socket_lock, "socket");

    portno = 4444;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) panic("error opening socket");

    server = gethostbyname("dawgwood");

    if (server == NULL) panic("socket error, no such host\n");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        panic("error connecting to socket");

}

