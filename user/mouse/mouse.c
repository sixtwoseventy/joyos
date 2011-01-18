//CHANGE ROBOT_ID TO EITHER 128 OR 129
//CHANGE IN THE MAKEFILE WHAT THE OTHER
//SOURCE FILE IS TO DETERMINE BEHAVIOR
//GO HOME: "runaway.c"
//STATIONARY: "stationary.c"
//PATROL: "patrol.c"

#include "runaway.h"

//ID defined by frob
//frob < 512, 128
//from >= 512, 129
//#define ROBOT_ID 128

#include <joyos.h>
#include <math.h>
#include <lib/pid.h>

#define RIGHT_MOTOR 0
#define LEFT_MOTOR 1

#define GYRO_PORT       11
#define LSB_US_PER_DEG  -1400000*365.0/360.0

#define MaxVelocity 150
#define TARGET_VELOCITY 80
#define TURNING_SCALE .8

#define ACTIVATE_THRESH 600
#define DEACTIVATE_THRESH 400
#define DEACTIVATE_SAMPLES 10
#define START_RAMPDOWN 5

#define RINGKP .02
#define RINGKD 0
#define RINGKI 0
#define PEAK_STRENGTH 2000

//PID

//10 2 0 0
#define PID_DELAY 10
#define KP 1.5
#define KD 0
#define KI 0

#define TURN_THRESH 60*KP
#define CUTOFF_ANGLE 45

uint8_t Compass = 0x42;
uint8_t ReadCommand[] = {'A'};
uint8_t ResetCommand[] = {'O'};
uint8_t OutputAngleCommand[] = {'G',0x4E,0x00};
uint8_t OutputMagXCommand[] = {'G',0x4E,0x03};
uint8_t OutputMagYCommand[] = {'G',0x4E,0x04};
uint8_t Data[2];
uint16_t AngleReading;
int16_t MagXRawReading;
int16_t MagXReading;
int16_t MagYReading;
uint8_t was_overload;
uint32_t old_time;
float TargetVelocity = 0;
float FieldStrength = 0;
uint16_t PIDDelay;
struct pid_controller ForwardPID;
volatile uint8_t isActivated=0;
uint8_t isForward;
struct pid_controller RingPID;

float NearbyScale = 1.0f;
//.7
#define NEARBY_DAMP .5
#define NEARBY_RAMP 1.2
#define NEARBY_MIN .2

#define velAlpha .8
#define DEAD_ZONE 5

float fangle;

float getError();

int usetup (void) {

    extern volatile uint8_t robot_id;
    robot_id = frob_read()<512?128:129;
    extern volatile uint8_t light_port;
    light_port = 2;
    printf("\nPlace robot,    press go.");
    go_click ();
    printf ("\nStabilizing...");
    pause (500);
    printf ("\nCalibrating     offset...\n");
    gyro_init (GYRO_PORT, LSB_US_PER_DEG, 500L);
    return 0;
}

void SetTargetVelocity(float vel) {
    TargetVelocity = velAlpha*vel+(1-velAlpha)*TargetVelocity;
}

int CaughtThread(){
    while(1){
        rf_status_update(isActivated);
        pause(100);
    }
    return 0;
}

float BoundedVelocity(float ProposedVelocity)
{
    if (ProposedVelocity < -MaxVelocity)
    {
        return -MaxVelocity;
    }
    else if (ProposedVelocity > MaxVelocity)
    {
        return MaxVelocity;
    }
    else
    {
        return ProposedVelocity;
    }
}

float FixDeadZone(float vel){
    if (vel < 0.) return vel - DEAD_ZONE;
    else return vel + DEAD_ZONE;
}

void SetRightVelocity(float vel)
{
    motor_set_vel(RIGHT_MOTOR, (int16_t)FixDeadZone(vel)); }

void SetLeftVelocity(float vel)
{
    motor_set_vel(LEFT_MOTOR, (int16_t)FixDeadZone(vel)); }


void ForwardApplyMV(float MV)
{
    float angle = getError();
    if (isForward) {
        if (angle < -CUTOFF_ANGLE) {
            SetRightVelocity(TURNING_SCALE*TargetVelocity);
            SetLeftVelocity(-TURNING_SCALE*TargetVelocity);
        } else if (angle > CUTOFF_ANGLE) {
            SetRightVelocity(-TURNING_SCALE*TargetVelocity);
            SetLeftVelocity(TURNING_SCALE*TargetVelocity);
        } else {
            SetRightVelocity(BoundedVelocity(TargetVelocity+NearbyScale*MV/2));
            SetLeftVelocity(BoundedVelocity(TargetVelocity-NearbyScale*MV/2));
        }
    } else {
        if (angle < -CUTOFF_ANGLE) {
            SetRightVelocity(TURNING_SCALE*TargetVelocity);
            SetLeftVelocity(-TURNING_SCALE*TargetVelocity);
        } else if (angle > CUTOFF_ANGLE) {
            SetRightVelocity(-TURNING_SCALE*TargetVelocity);
            SetLeftVelocity(TURNING_SCALE*TargetVelocity);
        } else {
            SetRightVelocity(BoundedVelocity(-TargetVelocity+NearbyScale*MV/2));
            SetLeftVelocity(BoundedVelocity(-TargetVelocity-NearbyScale*MV/2));
        }
    }
}

void MotorCoast(uint8_t Motor)
{
    motor_set_vel(Motor, 0);
}

void CoastMotors()
{
    MotorCoast(LEFT_MOTOR);
    MotorCoast(RIGHT_MOTOR);
}

float
strength(int16_t MagX, int16_t MagY){
    float fMagX = (float)(MagX);
    float fMagY = (float)(MagY);

    return sqrt(fMagX*fMagX+fMagY*fMagY);
}

/*int16_t bounded_vel(float vel){
  if (vel > 150.0) return 150;
  else if (vel < 66.6) return 67;
  else return (int16_t)vel;
  }*/

void MagnetInit() {
    old_time = get_time();

    was_overload=0;

    i2cInit();

    i2cMasterSend(Compass,1,ResetCommand);
    pause(10);
}

float ComputeMagnetTarget() {
    /* Reset periodically */

    /*if ((get_time() - old_time) > 250){
            old_time = get_time();
            i2cMasterSend(Compass,1,ResetCommand);
            pause(10);

            }*/
    i2cMasterSend(Compass,1,ResetCommand);
    pause(10);


    /* Read Heading Information */

    i2cMasterSend(Compass,3,OutputAngleCommand);
    i2cMasterSend(Compass,1,ReadCommand);

    pause(10);

    i2cMasterReceive(Compass,2,Data);
    AngleReading = (Data[0] << 8) | Data[1];

    /* Read Offset-Corrected Magnetometer X */

    i2cMasterSend(Compass,3,OutputMagXCommand);
    i2cMasterSend(Compass,1,ReadCommand);

    pause(10);

    i2cMasterReceive(Compass,2,Data);
    MagXReading = (Data[0] << 8) | Data[1];

    /* Read Offset-Corrected Magnetometer X */

    i2cMasterSend(Compass,3,OutputMagYCommand);
    i2cMasterSend(Compass,1,ReadCommand);

    pause(10);

    i2cMasterReceive(Compass,2,Data);
    MagYReading = (Data[0] << 8) | Data[1];

    //printf("\nX:%.1f,%.1f, Y:%.1f,%.1f",MagXReading/10.0,MagXRawReading/10.0,MagYReading/10.0,MagYRawReading/10.0);
    //printf("\n(%.1f,%.1f)",MagXReading/10.0,MagYReading/10.0);

    FieldStrength = strength(MagXReading,MagYReading);
    /*
       if (FieldStrength > ACTIVATE_THRESH){
       was_overload = 1;
       }
       else if (was_overload && (FieldStrength < DEACTIVATE_THRESH)){
       was_overload = 0;
       old_time = get_time();
       i2cMasterSend(Compass,1,ResetCommand);
       pause(10);
       }
     */
    fangle = ((float)AngleReading)/10.0;
    if (fangle > 180.0) fangle -= 360.0;

    //printf("\nangle:%.1f, strength: %.2f",fangle, strength(MagXReading,MagYReading));


    //i2cMasterSend(Compass,1,ResetCommand);
    //pause(10);

    return fangle;
}

float getError() {
    if (isForward) {
        return -fangle;
    } else {
        return (-(int)fangle+360)%360-180;
    }
}

float getStrengthError() {
    float error = PEAK_STRENGTH - FieldStrength;
    if (error < 0) {
        return 0;
    } else {
        return -error;
    }
}

void applyNearbyScale(float val) {
    if (val > 1.0f) {
        NearbyScale = 1.0f;
    } else if (val < NEARBY_MIN) {
        NearbyScale = 0.0f;
    } else {
        NearbyScale = val;
    }
    /*
       if (val > 1.0f) {
       NearbyScale = 1.0f;
       } else if (val <= .25f) {
//NearbyScale = 0.0f;
float possibleVal = NearbyScale * .8;
NearbyScale = (val > possibleVal) ? val : possibleVal;
//NearbyScale *= .8;
} else {
NearbyScale = val;
}*/
}

void PIDInit() {
    SetTargetVelocity(TARGET_VELOCITY);

    printf("\n");

    init_pid(&ForwardPID,
            (float)KP,
            (float)KI,
            (float)KD,
            &getError,
            &ForwardApplyMV);
    ForwardPID.goal = 0.0;
    ForwardPID.enabled = 1;

    init_pid(&RingPID,
            RINGKP,
            RINGKD,
            RINGKI,
            &getStrengthError,
            &applyNearbyScale);
    RingPID.goal = 0.0;
    RingPID.enabled = 1;
}

void PIDUpdate() {
    update_pid(&ForwardPID);
    update_pid(&RingPID);
    //ForwardPID.goal = target;
    //pause(PID_DELAY);
}

void PIDReset() {
    init_pid(&ForwardPID,
            (float)KP,
            (float)KD,
            (float)KI,
            &getError,
            &ForwardApplyMV);
    ForwardPID.goal = 0.0;
    ForwardPID.enabled = 1;

    init_pid(&RingPID,
            RINGKP,
            RINGKD,
            RINGKI,
            &getStrengthError,
            &applyNearbyScale);
    RingPID.goal = 0.0;
    RingPID.enabled = 1;
}

/**
 * Run all tests
 */
int
umain (void) {

    initRun();
    isActivated = false;
    isForward = true;
    MagnetInit();
    PIDInit();
    uint8_t num_low_readings = 0;
    activateRun();

    create_thread(&CaughtThread, STACK_DEFAULT, 0, "Caught Thread");

    while(1){

        //Read Magnet data
        ComputeMagnetTarget();

        //uint8_t oldForward = isForward;

        if (fangle > 90 || fangle < -90) {
            isForward = false;
        } else {
            isForward = true;
        }

        //Determine if we should change from activated->deactivated or vice versa
        if (isActivated) {
            if (FieldStrength < DEACTIVATE_THRESH) {
                num_low_readings++;
                //Need START_RAMPDOWN readings to begin motor rampdown
                if (num_low_readings > START_RAMPDOWN) {
                    SetTargetVelocity(TARGET_VELOCITY*(1.0-(float)(num_low_readings-START_RAMPDOWN)/(float)(DEACTIVATE_SAMPLES-START_RAMPDOWN)));
                    //SetTargetVelocity(TARGET_VELOCITY);
                }
                //If you have DEACTIVATE_SAMPLES low readings, deactivate
                if (num_low_readings == DEACTIVATE_SAMPLES) {
                    isActivated = false;
                    num_low_readings = 0;
                    SetTargetVelocity(TARGET_VELOCITY);
                    CoastMotors();
                    printf("\nDEACTIVATED");
                }
            } else {
                //We had a good reading, reset bad reading count and motor velocity
                num_low_readings = 0;
                SetTargetVelocity(TARGET_VELOCITY);
            }
        } else {
            //Activate on a single high reading
            if (FieldStrength > ACTIVATE_THRESH) {
                isActivated = true;
                SetTargetVelocity(TARGET_VELOCITY);
                printf("\nACTIVATED");
                activateRun();
                PIDReset();
            }
        }

        SetTargetVelocity(NearbyScale*TargetVelocity);

        //Stop if not activated, otherwise update PID
        if (!isActivated) {
            step();
        } else {
            PIDUpdate();
        }

        //pause(100);

    }

    return 0;

}
