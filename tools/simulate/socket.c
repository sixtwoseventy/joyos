#include "socket.h"

// TODO move to motor file
void motor_set_vel(int mot, int vel){
    sprintf(socket_buffer, "m %d %d\n", mot, vel);
	write(sockfd, socket_buffer, strlen(socket_buffer));
}

// TODO move to gyro file
float gyro_get_degrees(){

	float in;

	write(sockfd, "a\n", 2);
	read(sockfd, socket_buffer, SOCKET_BUF_SIZE);
	sscanf(socket_buffer, "%f", &in);
  
	return in;

}

// TODO replace functionality with bump sensors
int isStuck(){

	write(sockfd, "s\n", 2);
	read(sockfd, socket_buffer, SOCKET_BUF_SIZE);

    return socket_buffer[0] == 't';
  
}

// TODO replace with panic
void error(char *msg)
{
  perror(msg);
  exit(0);
}

void init_socket(){

    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = 4444;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
        error("ERROR opening socket");

    server = gethostbyname("dawgwood");

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

}

