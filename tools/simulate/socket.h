#ifndef __SOCKET_H_
#define __SOCKET_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define SOCKET_BUF_SIZE 256

// socket i/o variables 
int sockfd;
char socket_buffer[SOCKET_BUF_SIZE];

// TODO move to motor file
void motor_set_vel(int mot, int vel);

// TODO move to gyro file
float gyro_get_degrees();

// TODO replace functionality with bump sensors
int isStuck();

// TODO replace with panic
void error(char *msg);

void init_socket();

#endif

