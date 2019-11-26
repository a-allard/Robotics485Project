#include "RExDriveSystem.h"

#define DEADVOLT 140 // PWM value [0:255] that barely turns on the motors

// declare IR reflective sensors


// declare motors (pinA, pinB, pinE)
Motor Left(0, 1, 2);
Motor Right(3, 4, 5);
Motor Front(6, 7, 8);
Motor Back(9, 10, 11);

OmniwheelDriveSys REx(Front, Back, Left, Right); 

// function prototypes
void stopAllActions(void);
void lineFollowingStatus(void);
void lineFollowingMode(bool);
void reportVelocity(void);
void setVelocity(float,float,float);
void askMsgAgain(void);
void followLine(void);

String message;
float command[4] = {0.0, 0.0, 0.0, 0.0};
bool cmd_flag = false;
bool followLineMode = false;

float setVel, measVel=0;
float setDir, measDir=0;
float setOri, measOri=0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Serial.println("My name is REx. Hear me ROAR!\n");
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
  Serial.print(measVel);
  Serial.print(",");
  Serial.print(measDir);
  Serial.print(",");
  Serial.println(measOri);
  Serial.println("0");
}

// use recieved data to set current velocity
// x & y exist in range [-100%,100%], and theta from [-pi, pi]
void setVelocity(float xVel, float yVel, float theta) {
  // setVel += goalVel - measVel;
  // setDir += goalDir - measDir;
  // setOri += goalOri - measOri;
  float speed = 0;
  if (xVel > 0) {
    speed = map((int)xVel, 0, 100, DEADVOLT, 255);
    REx.driveRight(speed);
  } else if (xVel < 0) {
    speed = map((int)xVel*(-1), 0, 100, DEADVOLT, 255);
    REx.driveLeft(speed);
  } else {
    REx.driveLeft(0);
  }

  if (yVel > 0) {
    speed = map((int)yVel, 0, 100, DEADVOLT, 255);
    REx.driveForward(speed);
  } else if (yVel < 0) {
    speed = map((int)yVel*(-1), 0, 100, DEADVOLT, 255);
    REx.driveBackward(speed);
  } else {
    REx.driveForward(0);
  }

  if (theta > 0) {
    speed = map(theta, 0, 100, DEADVOLT, 255);
    REx.rotateCCW(speed);
  } else if (theta < 0) {
    speed = map(theta*(-1), 0, 100, DEADVOLT, 255);
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