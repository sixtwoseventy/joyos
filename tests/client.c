#include <joyos.h>
#include <math.h>
#include <stdio.h>

// robot port assignments
#define LEFT_MOTOR 0 
#define RIGHT_MOTOR 1

void turnLeft(){
  float startTheta = gyro_get_degrees();
  motor_set_vel(LEFT_MOTOR, -50);
  motor_set_vel(RIGHT_MOTOR, 50);
  while((gyro_get_degrees() - startTheta) < M_PI/2.){
    pause(25);
  }
}

void forward(){
  motor_set_vel(LEFT_MOTOR, 255);
  motor_set_vel(RIGHT_MOTOR, 255);
}

void backward(){
  motor_set_vel(LEFT_MOTOR, -255);
  motor_set_vel(RIGHT_MOTOR, -255);
}

int usetup(){

	return 0;

}

int umain(){
  int sign = 1;
  
  while(1){
    
    if (isStuck()){
		backward();
		pause(500);
		turnLeft();
	}
  
    forward();
    
    pause(100);
  
  }
}

