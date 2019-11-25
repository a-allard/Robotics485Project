#include "RExDriveSystem.h"

// declare IR reflective sensors


// declare motors (pinA, pinB, pinE)
Motor Right(7, 8, 9);
Motor Left(10, 11, 12);
Motor Back(13, 14, 15);
Motor Front(16, 17, 18);

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

float setVel, measVel;
float setDir, measDir;
float setOri, measOri;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (cmd_flag) {
    // a new command has been received from the RPI
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
void setVelocity(float goalVel, float goalDir, float goalOri) {
  setVel += goalVel - measVel;
  setDir += goalDir - measDir;
  setOri += goalOri - measOri;
  Serial.println("0");
}

// the command received was unknown
void askMsgAgain(void) {
  Serial.println("1"); 
}

void followLine(void) {
  return;
}