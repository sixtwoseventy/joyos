#include <joyos.h>
#include <math.h>
#include <lib/pid.h>

#define DRIVE_KP 2.0
#define DRIVE_KD 0.0
#define DRIVE_KI 0.0

#define RIGHT_MOTOR 0
#define LEFT_MOTOR 1
#define MAX_VEL 150

#define TURN_VEL 60
#define RUN_CUTOFF_ANGLE 30

#define CLAMP(X) X>0?(X>MAX_VEL?MAX_VEL:X):0
#define REV_CLAMP(X) X<0?(X<-MAX_VEL?-MAX_VEL:X):0
#define RF_TO_DEGREES(X) (float)(X*180.0/2048.0)
#define RAD_TO_DEGREES(X) (float)(X*180.0/M_PI)
#define CORRECT_ANGLE(X) (X>0?(X+180)%360-180:(X+540)%360-180)
#define RF_TO_INCHES(X) X*96.0/4096.0
#define MIN(X,Y) X<Y?X:Y

#define BOT1 1
#define BOT2 objects[2].id<128?2:3

int corners_x[]= {-1024,-1024,1024,1024,0,0};
int corners_y[] = {0,512,0,512,0,512};
#define NUM_WAYPOINTS 6

typedef enum {
    TURNING,
    DRIVING,
    INIT,
    ACTIVATED,
    HALT,
    NAVIGATE,
    ACTIVATE_PENDING,
    DEFEND, //Stay at your current position
    NEXT_WAYPOINT
} state_enum;

state_enum state = INIT;

int x = 0;
int y = 0;
float angle; //CW about +y

int target_x = 0;
int target_y = 0;
float angle_to_target = 0;

struct pid_controller driver;

int targetVel = 100;

uint32_t state_time = 0;

uint32_t last_update_time = 0;

bool isForward = true;

void updateSelfPosition(bool update_gyro);
void determineTargetPosition();
void updateAngleToTarget();

uint8_t current_position = 0;
#define NUM_POSITIONS 2

float runGetError() {
    int e = CORRECT_ANGLE((int)(gyro_get_degrees() - angle_to_target));
    if (e < 90 && e > -90) {
        isForward = true;
        return (float)e;
    } else {
        isForward = false;
        return (float)CORRECT_ANGLE(e+180);
    }
}

void turnAndDrive(float MV)
{
    int angle = (int)runGetError();
    //printf("\n%i",angle);
    if (!isForward) {
        //printf("\n->%i",angle);
        if (angle < -RUN_CUTOFF_ANGLE) {
            //printf("\nTURNRIGHT");
            motor_set_vel(RIGHT_MOTOR,-TURN_VEL);
            motor_set_vel(LEFT_MOTOR,TURN_VEL);
        } else if (angle > RUN_CUTOFF_ANGLE) {
            //printf("\nTURNLEFT");
            motor_set_vel(RIGHT_MOTOR,TURN_VEL);
            motor_set_vel(LEFT_MOTOR,-TURN_VEL);
        } else {
            //printf("\nSTRAIGHT, %.2f", MV);
            motor_set_vel(RIGHT_MOTOR,CLAMP(targetVel-(int)MV));
            motor_set_vel(LEFT_MOTOR,CLAMP(targetVel+(int)MV));
        }
    } else {
        //printf("\nOTHER");
        if (angle < -RUN_CUTOFF_ANGLE) {
            motor_set_vel(RIGHT_MOTOR,-TURN_VEL);
            motor_set_vel(LEFT_MOTOR,TURN_VEL);
        } else if (angle > RUN_CUTOFF_ANGLE) {
            motor_set_vel(RIGHT_MOTOR,TURN_VEL);
            motor_set_vel(LEFT_MOTOR,-TURN_VEL);
        } else {
            motor_set_vel(RIGHT_MOTOR,REV_CLAMP(-targetVel-(int)MV));
            motor_set_vel(LEFT_MOTOR,REV_CLAMP(-targetVel+(int)MV));
        }
    }
}

void resetPID(float target) {
    init_pid(&driver,
            DRIVE_KP,
            DRIVE_KI,
            DRIVE_KD,
            &runGetError,
            &turnAndDrive);
    driver.goal = 0;
    driver.enabled = 1;
}

float getTargetAngle(int x1, int y1) {
    //Argument order switched to create CW angle about +y
    return RAD_TO_DEGREES(atan2((float)(x1-x),(float)(y1-y)));
}

float getDistanceToTarget(int x1, int y1) {
    float diffx = (float)(x1-x);
    float diffy = (float)(y1-y);
    return RF_TO_INCHES((float)sqrt(diffx*diffx+diffy*diffy));
}

float getDistanceGeneral(int x1, int y1, int x2, int y2) {
    float diffx = (float)(x1-x2);
    float diffy = (float)(y1-y2);
    return RF_TO_INCHES((float)sqrt(diffx*diffx+diffy*diffy));
}


bool navigateToTarget() {
    //return true if need to be called next frame

    if (get_time() - state_time > 2800) {
        determineTargetPosition();
        updateSelfPosition(true);
        updateAngleToTarget();
        state_time = get_time();
    }
    else if (get_time() - state_time > 2500) {
        motor_set_vel(RIGHT_MOTOR,0);
        motor_set_vel(LEFT_MOTOR,0);
        return true;
    }
    if (get_time() - last_update_time > 100) {
        updateSelfPosition(false);
        determineTargetPosition();
        updateAngleToTarget();
    }
    if (getDistanceToTarget(target_x,target_y) < 4.0) {
        motor_set_vel(RIGHT_MOTOR,0);
        motor_set_vel(LEFT_MOTOR,0);
        resetPID(0);
        return false;
    }
    update_pid(&driver);
    return true;
}

void updateSelfPosition(bool updateGyro) {
    //Updates position from RF if new data has been received
    if (x != objects[0].x && y != objects[0].y) {
        x = objects[0].x;
        y = objects[0].y;
        if (updateGyro) {
            angle = RF_TO_DEGREES(objects[0].theta);
            gyro_set_degrees(angle);
        }
    }
    last_update_time = get_time();
}

void updateAngleToTarget() {
    angle_to_target = getTargetAngle(target_x,target_y);
    //driver.goal = angle_to_target;
}

void determineTargetPosition() {
    //Currently, chase after mouse bot.
    /*if (current_position == 0) {
      target_x = 1024;//objects[2].x;
      target_y = -1024;//objects[2].y;
      } else {
      target_x = 1024;
      target_y = 1024;
      }*/
    //TOP MID
    //target_x = (objects[BOT1].x + objects[BOT2].x)/2;
    //target_y = (objects[BOT1].y + objects[BOT2].y)/2;

    float max_w = 0;
    uint8_t best = 0;


    float w;

    for (uint8_t i = 0; i < NUM_WAYPOINTS; i++) {
        w = MIN(getDistanceGeneral(objects[BOT1].x,objects[BOT1].y,corners_x[i],corners_y[i]),
                getDistanceGeneral(objects[BOT2].x,objects[BOT2].y,corners_x[i],corners_y[i]));
        if (w > max_w) { best = i; max_w = w; }
    }

    target_x = corners_x[best];
    target_y = corners_y[best];
    //rf_printf("(%i, %i)\n", target_x, target_y);
    /*
       if (current_position == 0) {
       target_x = 1024;//objects[2].x;
       target_y = 0;//objects[2].y;
       } else {
       target_x = -1024;
       target_y = 0;
       }*/

    //TOP HIGH
    /*
       if (current_position == 0) {
       target_x = 1024;
       target_y = 1024;
       } else {
       target_x = -1024;
       target_y = 1024;
       }*/
}
/*

   void rfUpdate(bool updateGyro = true) {
   updateSelfPosition(updateGyro);
   last_update_time = get_time();
   }
 */

void initRun() {
    /*
     * Called upon the mouse entering run away mode.
     */
    state = INIT;
    state_time = get_time();
    determineTargetPosition();
    return;
}

void activateRun() {
    /*
     * Called upon the mouse entering run away mode.
     */
    state = ACTIVATE_PENDING;
    state_time = get_time();
    return;
}

void step() {
    switch(state) {
        case ACTIVATE_PENDING:
            if ((get_time() - state_time) > 750) {
                state_time = get_time();
                state = ACTIVATED;
            }
            break;
        case ACTIVATED:
            updateSelfPosition(true);
            determineTargetPosition();
            updateAngleToTarget();
            state = NAVIGATE;
            resetPID(angle_to_target);
            state_time = get_time();
            break;
        case NAVIGATE:
            navigateToTarget();
            break;
        default:
            //rf_printf("Default\n");
            break;
    }
    //update_pid(&driver);
    //pause(50);
}
