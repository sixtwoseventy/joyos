#include "socket.h"

// robot port assignments
int leftMotor = 0, rightMotor = 1;

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

void usetup(){

	init_socket();

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
