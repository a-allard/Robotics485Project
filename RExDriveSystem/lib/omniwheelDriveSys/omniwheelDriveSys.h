#ifndef OMNIDRIVE
#define OMNIDRIVE

#include <Arduino.h>
#include "../HBridgeMotor/HBridgeMotor.h"

class OmniwheelDriveSys {
private:
    int pwmF=0, pwmB=0, pwmL=0, pwmR=0;
    int linX=0, linY=0;
    int ang=0;
public:
    Motor Front, Back, Left, Right;
    OmniwheelDriveSys(void) {
        Motor Right(7, 8, 9);
        Motor Left(10, 11, 12);
        Motor Back(13, 14, 15);
        Motor Front(16, 17, 18);
    }
    OmniwheelDriveSys(Motor &F, Motor &B, Motor &L, Motor &R) {
        Front = F;
        Back = B;
        Left = L;
        Right = R;
    }
    void driveForward(int speed) {
        linY = speed;
        pwmL = max(-linY + ang, -255);
        pwmR = min(linY + ang, 255);
        Left.velocity(pwmL);
        Right.velocity(pwmR);
    }
    void driveBackward(int speed) {
        linY = -speed;
        pwmL = min(-linY + ang, 255);
        pwmR = max(linY + ang, 255);
        Left.velocity(pwmL);
        Right.velocity(pwmR);
    }
    void driveLeft(int speed) {
        linX = -speed;
        pwmF = min(-linX + ang, 255);
        pwmB = max(linX + ang, -255);
        Front.velocity(pwmF);
        Back.velocity(pwmB);
    }
    void driveRight(int speed) {
        linX = speed;
        pwmF = max(-linX + ang, -255);
        pwmB = min(linX + ang, 255);
        Front.velocity(pwmF);
        Back.velocity(pwmB);
    }
    void rotateCW(int speed) {
        ang = speed;
        pwmF = min(-linX + ang, 255);
        pwmB = min(linX + ang, 255);
        pwmL = min(-linY + ang, 255);
        pwmR = min(linY + ang, 255);
        Front.velocity(pwmF);
        Back.velocity(pwmB);
        Left.velocity(pwmL);
        Right.velocity(pwmR);
    }
    void rotateCCW(int speed) {
        ang = -speed;
        pwmF = max(-linX + ang, -255);
        pwmB = max(linX + ang, -255);
        pwmL = max(-linY + ang, -255);
        pwmR = max(linY + ang, -255);
        Front.velocity(pwmF);
        Back.velocity(pwmB);
        Left.velocity(pwmL);
        Right.velocity(pwmR);
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