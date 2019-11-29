#ifndef OMNIDRIVE
#define OMNIDRIVE

#include <Arduino.h>
#include "../HBridgeMotor/HBridgeMotor.h"


class OmniwheelDriveSys {
private:
    float setVelF, goalVelF, measVelF;
    float setVelB, goalVelB, measVelB;
    float setVelL, goalVelL, measVelL;
    float setVelR, goalVelR, measVelR;
    int linX=0, linY=0;
    int ang=0;
public:
    Motor Front, Back, Left, Right;
    OmniwheelDriveSys(void) {
        // declare motors (pinA, pinB, pinE, pinEncA, pinEncB)
        Left = Motor(0, 1, 2, 14, 15);
        Right = Motor(3, 4, 5, 16, 17);
        Front = Motor(6, 7, 8, 18, 19);
        Back = Motor(9, 10, 11, 20, 21);
    }
    OmniwheelDriveSys(Motor &F, Motor &B, Motor &L, Motor &R) {
        Front = F;
        Back = B;
        Left = L;
        Right = R;
    }
    void driveForward(int speed) {
        linY = speed;
        goalVelL = max(-linY + ang, -255);
        goalVelR = min(linY + ang, 255);
        //Left.setVelocity(goalVelL);
        //Right.setVelocity(goalVelR);
        measVelL = Left.readVelocity();
        measVelR = Right.readVelocity();
        setVelL += goalVelL - measVelL*255;
        setVelR += goalVelR - measVelR*255;
        Left.setVelocity(setVelL);
        Right.setVelocity(setVelR);
    }
    void driveBackward(int speed) {
        linY = -speed;
        goalVelL = min(-linY + ang, 255);
        goalVelR = max(linY + ang, 255);
        measVelL = Left.readVelocity();
        measVelR = Right.readVelocity();
        setVelL += goalVelL - measVelL*255;
        setVelR += goalVelR - measVelR*255;
        Left.setVelocity(setVelL);
        Right.setVelocity(setVelR);
    }
    void driveLeft(int speed) {
        linX = -speed;
        goalVelF = min(-linX + ang, 255);
        goalVelB = max(linX + ang, -255);
        measVelF = Front.readVelocity();
        measVelB = Back.readVelocity();
        setVelF += goalVelF - measVelF*255;
        setVelB += goalVelB - measVelB*255;
        Front.setVelocity(setVelF);
        Back.setVelocity(setVelB);
    }
    void driveRight(int speed) {
        linX = speed;
        goalVelF = max(-linX + ang, -255);
        goalVelB = min(linX + ang, 255);
        measVelF = Front.readVelocity();
        measVelB = Back.readVelocity();
        setVelF += goalVelF - measVelF*255;
        setVelB += goalVelB - measVelB*255;
        Front.setVelocity(setVelF);
        Back.setVelocity(setVelB);
    }
    void rotateCW(int speed) {
        ang = speed;
        goalVelF = min(-linX + ang, 255);
        goalVelB = min(linX + ang, 255);
        goalVelL = min(-linY + ang, 255);
        goalVelR = min(linY + ang, 255);
        measVelL = Left.readVelocity();
        measVelR = Right.readVelocity();
        measVelF = Front.readVelocity();
        measVelB = Back.readVelocity();
        setVelL += goalVelL - measVelL*255;
        setVelR += goalVelR - measVelR*255;
        setVelF += goalVelF - measVelF*255;
        setVelB += goalVelB - measVelB*255;
        Front.setVelocity(setVelF);
        Back.setVelocity(setVelB);
        Left.setVelocity(setVelL);
        Right.setVelocity(setVelR);
    }
    void rotateCCW(int speed) {
        ang = -speed;
        goalVelF = max(-linX + ang, -255);
        goalVelB = max(linX + ang, -255);
        goalVelL = max(-linY + ang, -255);
        goalVelR = max(linY + ang, -255);
        measVelL = Left.readVelocity();
        measVelR = Right.readVelocity();
        measVelF = Front.readVelocity();
        measVelB = Back.readVelocity();
        setVelL += goalVelL - measVelL*255;
        setVelR += goalVelR - measVelR*255;
        setVelF += goalVelF - measVelF*255;
        setVelB += goalVelB - measVelB*255;
        Front.setVelocity(setVelF);
        Back.setVelocity(setVelB);
        Left.setVelocity(setVelL);
        Right.setVelocity(setVelR);
    }
    void activeStop(void) {
        Front.activeStop();
        Back.activeStop();
        Left.activeStop();
        Right.activeStop();
    }
    void activeStopHoriz(void) {
        Front.activeStop();
        Back.activeStop();
    }
    void activeStopVert(void) {
        Left.activeStop();
        Right.activeStop();
    }
    void passiveStop(void) {
        Front.passiveStop();
        Back.passiveStop();
        Left.passiveStop();
        Right.passiveStop();
    }
    void passiveStopHoriz(void) {
        Front.passiveStop();
        Back.passiveStop();
    }
    void passiveStopVert(void) {
        Left.passiveStop();
        Right.passiveStop();
    }
};

#endif