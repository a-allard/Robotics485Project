#include "RExDriveSystem.h"


// Allard changed from 140 to 130 11/29/2019
#define DEADVOLT 130 // PWM value [0:255] that barely turns on the motors


//Encoder leftEncoder(16, 17), rightEncoder(23, 22), frontEncoder(20, 21), backEncoder(14, 15);
//Encoder backEncoder(16, 17), frontEncoder(23, 22), rightEncoder(20, 21), leftEncoder(14, 15);
Encoder frontEncoder(16, 17), backEncoder(23, 22), leftEncoder(20, 21), rightEncoder(14, 15);
volatile float leftVel = 0, rightVel = 0, frontVel = 0, backVel = 0;
/*
Motor Left(0, 1, 2);
Motor Right(3, 4, 5);
Motor Front(6, 7, 8);
Motor Back(9, 10, 11);
*/

Motor Right(0, 1, 2);
Motor Left(3, 4, 5);
Motor Back(6, 7, 8);
Motor Front(9, 10, 11);

OmniwheelDriveSys REx(&Front, &Back, &Left, &Right);
IntervalTimer t;

// function prototypes
void stopAllActions(void);
void lineFollowingStatus(void);
void lineFollowingMode(bool);
void reportVelocity(void);
void setVelocity(float,float,float);
void askMsgAgain(void);
void followLine(void);
void getVels(void);
float mapD(float val, float fromA, float fromB, float toA, float toB);
float toMPS(float ticks);

String message;
float command[4] = {0.0, 0.0, 0.0, 0.0};
bool cmd_flag = false;
bool followLineMode = false;
const float RExVELHIGH = 0.3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Serial.println("My name is REx. Hear me ROAR!\n");
  t.begin(getVels, 10000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (cmd_flag) {
    // a new command has been received from the RPI
    //Serial.println(message);
    parseMsg(command, message);

    switch((int)command[0]) {
      case STP:
        stopAllActions();
        break;
      case LFS:
        lineFollowingStatus();
        break;
      case LFM:
        lineFollowingMode((bool)command[1]);
        break;
      case VELQUERY:
        reportVelocity();
        break;
      case VELSET:
        setVelocity(command[1], command[2], command[3]);
        break;
      default:
        askMsgAgain();
        break;
    }
    cmd_flag = false;

  } else if (followLineMode) {
    followLine();
  }

}

void serialEvent(void) {
  message = getSerialMsg();
  cmd_flag = (message.compareTo('0') ? true : false);
}

void stopAllActions(void) {
  REx.activeStop();
  Serial.println("0");
}

// reports the values on each IR sensor
void lineFollowingStatus(void) {
  Serial.println("0");
}

// sets whether or not Drive Teensy follows line
void lineFollowingMode(bool mode) {
  followLineMode = mode;
  Serial.println("0");
}

// Send velocity data to RPI
void reportVelocity(void) {
  // Serial.print(setVelF);
  // Serial.print(",");
  // Serial.print(setVelL);
  // Serial.print(",");
  // Serial.println(setVelR);
  Serial.println("ERR");
}

// use recieved data to set current velocity
// x & y exist in range [-100%,100%], and theta from [-pi, pi]
void setVelocity(float xVel, float yVel, float theta) {
  float speed = 0;
  //Serial.println("Setting vel");
  if (xVel > 0) {
    speed = mapD(xVel, 0.0, 100.0, 0.0, RExVELHIGH);
    REx.driveRight(speed);
  } else if (xVel < 0) {
    speed = mapD(xVel*(-1), 0.0, 100.0, 0.0, RExVELHIGH);
    REx.driveLeft(speed);
  } else {
    REx.driveLeft(0);
  }

  if (yVel > 0) {
    speed = mapD(yVel, 0.0, 100.0, 0.0, RExVELHIGH);
    REx.driveForward(speed);
    //Serial.println(speed);
  } else if (yVel < 0) {
    speed = mapD(yVel*(-1), 0, 100, 0, RExVELHIGH);
    REx.driveBackward(speed);
  } else {
    REx.driveForward(0);
  }
  //Serial.println(speed, 12);

  if (theta > 0) {
    speed = mapD(theta, 0, 100, 0, RExVELHIGH);
    REx.rotateCCW(speed);
  } else if (theta < 0) {
    speed = mapD(theta*(-1), 0, 100, 0, RExVELHIGH);
    REx.rotateCW(speed);
  } else {
    REx.rotateCW(0);
  }
  Serial.println("0");
}

// the command received was unknown
void askMsgAgain(void) {
  Serial.println("ERR"); 
}

void followLine(void) {
  return;
}

void getVels(){
  leftVel = toMPS((float)leftEncoder.readAndReset());
  backVel = toMPS((float)backEncoder.readAndReset());
  frontVel = toMPS((float)frontEncoder.readAndReset());
  rightVel = toMPS((float)rightEncoder.readAndReset());
  Left.updateVel(leftVel);
  Right.updateVel(rightVel);
  Front.updateVel(frontVel);
  Back.updateVel(backVel);
}

float toMPS(float ticks){
  return ticks / 12.0 / 0.01 / 75 *0.03;
}

float mapD(float val, float fromA, float fromB, float toA, float toB)
{
  return (val - fromA) * (toB - toA) / (fromB - fromA) + toA;
}