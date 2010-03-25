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

typedef enum {
    TURNING,
    DRIVING,
    INIT,
    ACTIVATED,
    HALT,
    NAVIGATE,
    ACTIVATE_PENDING,
    DEFEND //Stay at your current position
} state_enum;

state_enum state = INIT;

int x = 0;
int y = 0;
float angle; //CW about +y

int target_x = 0;
int target_y = 0;
float angle_to_target = 0;

struct pid_controller driver;

int targetVel = 40;

uint32_t state_time = 0;

uint32_t last_update_time = 0;

bool isForward = true;

void updateSelfPosition(bool update_gyro);
void determineTargetPosition();
void updateAngleToTarget();

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


bool navigateToTarget() {
    //return true if need to be called next frame

    if (get_time() - state_time > 3500) {
        updateSelfPosition(true);
        updateAngleToTarget();
        state_time = get_time();
    }
    else if (get_time() - state_time > 3000) {
        motor_set_vel(RIGHT_MOTOR,0);
        motor_set_vel(LEFT_MOTOR,0);
        return true;
    }
    if (get_time() - last_update_time > 500) {
        updateSelfPosition(false);
    }
    if (getDistanceToTarget(target_x,target_y) < 6.0) {
        motor_set_vel(RIGHT_MOTOR,0);
        motor_set_vel(LEFT_MOTOR,0);
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
    target_x = 1024;//objects[2].x;
    target_y = 0;//objects[2].y;
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
            if (!navigateToTarget()) {
                state = DEFEND;
            }
            break;
        case DEFEND:
            updateSelfPosition(false);
            if (getDistanceToTarget(target_x,target_y) > 6.0) {
                state = ACTIVATE_PENDING;
                state_time = get_time();
            }
            break;
        default:
            break;
    }
    //update_pid(&driver);
    pause(50);
}
