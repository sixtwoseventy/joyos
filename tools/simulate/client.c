#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 256

// robot port assignments
int leftMotor = 0, rightMotor = 1;

// socket i/o variables 
int sockfd;
char buffer[BUF_SIZE];

void motor_set_vel(int mot, int vel){
    sprintf(buffer, "m %d %d\n", mot, vel);
	write(sockfd, buffer, strlen(buffer));
}

float gyro_get_degrees(){

	float in;

	write(sockfd, "a\n", 2);
	read(sockfd, buffer, BUF_SIZE);
	sscanf(buffer, "%f", &in);
  
	return in;

}

int isStuck(){

	write(sockfd, "s\n", 2);
	read(sockfd, buffer, BUF_SIZE);

    return buffer[0] == 't';
  
}

void turnLeft(){
  float startTheta = gyro_get_degrees();
  motor_set_vel(leftMotor, -50);
  motor_set_vel(rightMotor, 50);
  while((gyro_get_degrees() - startTheta) < M_PI/2.){
    usleep(25000);
  }
}

void forward(){
  motor_set_vel(leftMotor, 255);
  motor_set_vel(rightMotor, 255);
}

void error(char *msg)
{
  perror(msg);
  exit(0);
}

void usetup(){

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

void umain(){
  int sign = 1;
  
  while(1){
    
    if (isStuck()) turnLeft();
  
    forward();
    
    usleep(100000);
  
  }
}

int main(){
	
	usetup();
	umain();

	return 0; // satisfy compiler
}
