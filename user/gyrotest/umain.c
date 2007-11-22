/*
 * The MIT License
 *
 * Copyright (c) 2007 MIT 6.270 Robotics Competition
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

/* Gyro demo program for gyro with noise injection.
   Howard Samuels, 11/16/2004 
   howard.samuels@analog.com
   
   Modified for Happyboard
   Ross Glashan, 07/05/2005
   rng@mit.edu
*/

#include <global.h>
#include <board.h>
#include <thread.h>
#include <math.h>

#define GYRO_PORT 11

#define MOTOR_LEFT 4
#define MOTOR_RIGHT 3
 
int gyro_type = 300;				/* 150 for ADXRS150, 300 for ADXRS300 */
float lsb_ms_per_deg = 2025.0;		/* nominal scale factor depending on gyro type */
float theta;                		/* current angle, units explained in update_angle() */
float offset;               		/* gyro offset */
uint32_t time_ms;               	/* system time the last gyro reading was taken */

void calibrate_gyro(void) {
/*
   Averaging a number of readings only works when the gyro 
   noise is greater than the converter resolution.  Since 
   the xrs300 noise is far lower than the 20mV resolution 
   of the 8-bit converter, a noise source has been added 
   to the gyro board.  You should average for about 5 
   seconds to minimize the effects of the added noise.
*/

  int samples;
  float sum;
  uint32_t start_time_ms;
  uint32_t int_time_ms = 5000L;

  printf("\nPlace robot,    press go.\n");
  goClick();
  printf("Stabilizing...\n");
  pause(500); /* Wait for robot to stabilize mechanically */
  printf("Calibrating     offset...\n");

  /* average some samples for offset */
  sum = 0.0;
  samples = 0;

  start_time_ms = get_time();
  while( ( get_time() - start_time_ms ) < int_time_ms ){
    sum += (float)analogRead(GYRO_PORT);
    samples++;
  }
  offset = sum / (float)samples;

  theta = 0.0;
  lcdClear();
  printf("Done calibration\n");
} /* calibrate_gyro() */

int update_angle(void) {
/* The units of theta are converter LSBs * ms
   1 deg/s = 5mV = 0.256 LSB for ADXRS300, 12.5mV = 0.64LSB for ADXRS150
   1 deg = 0.256LSB * 1000ms = 256 (stored in lsb_ms_per_deg)
   To convert theta to degrees, divide by lsb_ms_per_deg. */

  uint32_t delta_t_ms, new_time_ms, analog_value;

  time_ms = get_time();

  for(;;){
		analog_value = analogRead(GYRO_PORT);
    new_time_ms = get_time();
    delta_t_ms = (new_time_ms - time_ms);

    /* This does the Riemann sum; CCW gyro output polarity is negative
       when the gyro is visible from the top of the robot. */
    theta -= ((float)analog_value - offset) * (float)delta_t_ms; 

    time_ms = new_time_ms;
		yield();
  }
  return 0;
} /* update_angle() */

int display_angle(void) {
  for(;;){
    printf("%d degrees\n",(int)(theta / lsb_ms_per_deg));
    pause(500);
  }
  return 0;
} /* display_angle() */

void spin(int speed){
  motorSetVel(MOTOR_LEFT, -speed);
  motorSetVel(MOTOR_RIGHT, speed);
} /* spin() */

int sign(float a){
  if ( a > 0.0 ) return 1;
  else return -1;
} /* sign() */

void go_to_angle(float angle) {
/* To convert degrees to units of theta, multiply by lsb_ms_per_deg.
   Demo robot turns at about 120 deg/s when speed = 100 */
  float angle_target;
  float error, tolerance;

  tolerance = 1.0 * lsb_ms_per_deg;
  angle_target = angle * lsb_ms_per_deg;
  error = theta - angle_target;

  if ( fabs(error) > tolerance ) {

    spin(240 * -sign(error));
    while(fabs(error) > (80.0 * lsb_ms_per_deg)){
      error = theta - angle_target;
	  yield();
    }
    spin(200 * -sign(error));
    while(fabs(error) > (60.0 * lsb_ms_per_deg)){
      error = theta - angle_target;
	  yield();
    }
    spin(180 * -sign(error));
    while(fabs(error) > (10.0 * lsb_ms_per_deg)){
      error = theta - angle_target;
	  yield();
    }

    /* Fine adjust either direction as necessary */
    spin(160 * -sign(error));
    while(fabs(error) > tolerance){
      error = (theta - angle_target);
      spin(160 * -sign(error));
    }

    spin(0);  

  } /* if ( fabs(error)... */
} /* go_to_angle() */



void go_straight(float angle, int speed, long duration_ms){

/* The robot turns to the specified angle, then goes straight for 
   the specified duration. Note that speed must be greater than tweak, 
   and less than 255 - tweak. */

  uint32_t start_time;
  uint8_t tweak = 30;
  float angle_target;
  float tolerance;

  tolerance = lsb_ms_per_deg * 2.0; /* +/-0.5 degree accuracy */
  /* first turn the robot */
  go_to_angle(angle) ;

  /* then go forward under gyro control for the specified time */
  start_time = get_time();
  angle_target = angle * lsb_ms_per_deg; 
  
  while ((get_time() - start_time) < duration_ms){
    if (theta < (angle_target - tolerance)) {
      /* Turn CCW a little */
      motorSetVel(MOTOR_LEFT, speed - tweak);
      motorSetVel(MOTOR_RIGHT, speed + tweak);
    } else if (theta > (angle_target + tolerance)){
      /* Turn CW a little */
      motorSetVel(MOTOR_LEFT, speed + tweak);
      motorSetVel(MOTOR_RIGHT, speed - tweak);
    } else {
      motorSetVel(MOTOR_LEFT, speed);
      motorSetVel(MOTOR_RIGHT, speed);
    }  
    /* You can check a sensor here and terminate the while loop */
  } /* while */
  
  /* Throw it in reverse briefly to stop quickly */
  motorSetVel(MOTOR_LEFT, -255);
  motorSetVel(MOTOR_RIGHT, -255);
  pause(20L);  
  motorSetVel(MOTOR_LEFT, 0);
  motorSetVel(MOTOR_RIGHT, 0);  
} /* void go_straight() */


void go_open_loop( int speed, long duration_ms ){

/* Drive both motors at a specified speed and duration. */

  uint32_t start_time;

  start_time = get_time();
  
  while ((get_time() - start_time) < duration_ms){
    motorSetVel(MOTOR_LEFT, speed);
    motorSetVel(MOTOR_RIGHT, speed); 
    /* You can check a sensor here and terminate the while loop if necessary */
  } /* while */
  
  /* Throw it in reverse briefly to stop quickly */
  motorSetVel(MOTOR_LEFT, -100);
  motorSetVel(MOTOR_RIGHT, -100);
  pause(20L);  
  motorSetVel(MOTOR_LEFT, 0);
  motorSetVel(MOTOR_RIGHT, 0);
  
} /* void go_open_loop() */

int run_demo(void) {
  /* Make an Isoceles right triangle */
  go_straight(0.0,   220, 4000L);
  go_straight(135.0, 220, 5657L);   
  go_straight(270.0, 220, 4000L);
  go_to_angle(360.0); 

  /* Make another one, but obstruct the robot for the demo */
  goClick();
  pause( 500L );
  go_straight(360.0, 200, 4000L);
  go_straight(495.0, 200, 5657L); 
  go_straight(630.0, 200, 4000L);
  go_to_angle(720.0); 

  /* Drive straight in current direction under gyro control... */
  goClick();
  pause(500L);
  /* theta/lsb_ms_per_deg is the current angle in degrees */
  go_straight( theta/lsb_ms_per_deg, 200, 12000L );

  /* ... turn around ... */
  go_to_angle ( theta/lsb_ms_per_deg + 180.0 );

  /* ... and drive back open-loop (without gyro control). */
  go_open_loop( 200, 12000L ); 
  return 0;
}

// usetup is called during the calibration period. It must return before the
// period ends.
int usetup (void) {
	calibrate_gyro();
	return 0;
}

int umain(void) {
	create_thread(&update_angle, 64, 0, "update angle");
	create_thread(&display_angle, 64, 0, "display angle");
	create_thread(&run_demo, 64, 0, "demo");
	return 0;
}
