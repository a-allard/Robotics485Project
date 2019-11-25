#ifndef OMNIDRIVE
#define OMNIDRIVE

#include <Arduino.h>
#include "../HBridgeMotor/HBridgeMotor.h"

class OmniwheelDriveSys {
private:
    
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
        Left.spinCCW(speed);
        Right.spinCW(speed);
    }
    void driveBackward(int speed) {
        Left.spinCW(speed);
        Right.spinCCW(speed);
    }
    void driveLeft(int speed) {
        Front.spinCW(speed);
        Back.spinCCW(speed);
    }
    void driveRight(int speed) {
        Front.spinCCW(speed);
        Back.spinCW(speed);
    }
    void rotateCW(int speed) {
        Front.spinCCW(speed);
        Back.spinCCW(speed);
        Left.spinCCW(speed);
        Right.spinCCW(speed);
    }
    void rotateCCW(int speed) {
        Front.spinCW(speed);
        Back.spinCW(speed);
        Left.spinCW(speed);
        Right.spinCW(speed);
    }
    void activeStop(void) {
        Front.activeStop();
        Back.activeStop();
        Left.activeStop();
        Right.activeStop();
    }
    void passiveStop(void) {
        Front.passiveStop();
        Back.passiveStop();
        Left.passiveStop();
        Right.passiveStop();
    }
};

#endif