#ifndef HBRIDGEMOTOR
#define HBRIDGEMOTOR

#include <Arduino.h>

class Motor {
private:
    int pinA, pinB, pinE;
    bool stateA=HIGH; 
    bool stateB=HIGH;
    int  stateE=0;
    
public:
    // constructors
    Motor(void) {
        pinA = 0;
        pinB = 1;
        pinE = 2;
        pinMode(pinA, OUTPUT);
        pinMode(pinB, OUTPUT);
        pinMode(pinE, OUTPUT);
    }
    Motor(int A, int B, int E) {
        pinA = A;
        pinB = B;
        pinE = E;
        pinMode(pinA, OUTPUT);
        pinMode(pinB, OUTPUT);
        pinMode(pinE, OUTPUT);
    }
    int  getPinA(void) {return pinA;}
    int  getPinB(void) {return pinB;}
    int  getPinE(void) {return pinE;}
    bool getStateA(void) {return stateA;}
    bool getStateB(void) {return stateB;}
    int  getStateE(void) {return stateE;}

    void velocity(int speed) {
        if (speed > 0) {
            spinCW(speed);
        } else if (speed < 0) {
            spinCCW(-speed);
        } else {
            passiveStop();
        }
    }
    void spinCW(int speed) {
        stateA = HIGH;
        stateB = LOW;
        stateE = speed;
        digitalWrite(pinA, stateA);
        digitalWrite(pinB, stateB);
        analogWrite(pinE, stateE);
    }
    void spinCCW(int speed) {
        stateA = LOW;
        stateB = HIGH;
        stateE = speed;
        digitalWrite(pinA, stateA);
        digitalWrite(pinB, stateB);
        analogWrite(pinE, stateE);
    }
    void activeStop(void) {
        stateA = HIGH;
        stateB = HIGH;
        stateE = 0;
        digitalWrite(pinA, stateA);
        digitalWrite(pinB, stateB);
        analogWrite(pinE, stateE);
    }
    void passiveStop(void) {
        stateA = HIGH;
        stateB = HIGH;
        stateE = 255;
        digitalWrite(pinA, stateA);
        digitalWrite(pinB, stateB);
        analogWrite(pinE, stateE);
    }

    Motor & operator = (Motor M) {
        pinA = M.getPinA();
        pinB = M.getPinB();
        pinE = M.getPinE();
        stateA = M.getStateA();
        stateB = M.getStateB();
        stateE = M.getStateE();
        return *this;
    }
};

#endif 