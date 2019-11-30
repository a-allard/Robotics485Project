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
    float linX=0, linY=0;
    float ang=0;
    float maxIn = 0.5;
public:
    Motor *Front, *Back, *Left, *Right;
    OmniwheelDriveSys(void) {
        // declare motors (pinA, pinB, pinE, pinEncA, pinEncB)
        Left = new Motor(0, 1, 2);
        Right = new Motor(3, 4, 5);
        Front = new Motor(6, 7, 8);
        Back = new Motor(9, 10, 11);
    }
    OmniwheelDriveSys(Motor *F, Motor *B, Motor *L, Motor *R) {
        Front = F;
        Back = B;
        Left = L;
        Right = R;
    }
    
    void driveForward(float speed) {
        linY = speed;
        goalVelL = max(-linY + ang, -maxIn);
        goalVelR = min(linY + ang, maxIn);
        Left->setVelocity(goalVelL);
        Right->setVelocity(goalVelR);
    }
    void driveBackward(float speed) {
        linY = -speed;
        goalVelL = min(-linY + ang, maxIn);
        goalVelR = max(linY + ang, maxIn);
        Left->setVelocity(goalVelL);
        Right->setVelocity(goalVelR);
    }
    void driveLeft(float speed) {
        linX = -speed;
        goalVelF = min(-linX + ang, maxIn);
        goalVelB = max(linX + ang, maxIn);
        Front->setVelocity(goalVelF);
        Back->setVelocity(goalVelB);
    }
    void driveRight(float speed) {
        linX = speed;
        goalVelF = max(-linX + ang, -maxIn);
        goalVelB = min(linX + ang, maxIn);
        Front->setVelocity(goalVelF);
        Back->setVelocity(goalVelB);
    }
    void rotateCW(float speed) {
        ang = speed;
        goalVelF = min(-linX + ang, maxIn);
        goalVelB = min(linX + ang, maxIn);
        goalVelL = min(-linY + ang, maxIn);
        goalVelR = min(linY + ang, maxIn);
        Front->setVelocity(goalVelF);
        Back->setVelocity(goalVelB);
        Left->setVelocity(goalVelL);
        Right->setVelocity(goalVelR);
    }
    void rotateCCW(float speed) {
        ang = -speed;
        goalVelF = max(-linX + ang, -maxIn);
        goalVelB = max(linX + ang, -maxIn);
        goalVelL = max(-linY + ang, -maxIn);
        goalVelR = max(linY + ang, -maxIn);
        Front->setVelocity(goalVelF);
        Back->setVelocity(goalVelB);
        Left->setVelocity(goalVelL);
        Right->setVelocity(goalVelR);
    }
    void activeStop(void) {
        Front->activeStop();
        Back->activeStop();
        Left->activeStop();
        Right->activeStop();
    }
    void activeStopHoriz(void) {
        Front->activeStop();
        Back->activeStop();
    }
    void activeStopVert(void) {
        Left->activeStop();
        Right->activeStop();
    }
    void passiveStop(void) {
        Front->passiveStop();
        Back->passiveStop();
        Left->passiveStop();
        Right->passiveStop();
    }
    void passiveStopHoriz(void) {
        Front->passiveStop();
        Back->passiveStop();
    }
    void passiveStopVert(void) {
        Left->passiveStop();
        Right->passiveStop();
    }
};

#endif