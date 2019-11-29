#ifndef HBRIDGEMOTOR
#define HBRIDGEMOTOR

#include <Arduino.h>
#include "../Encoder/Encoder.h" 

#define WHEELRADIUS 0.03 // 2.5cm
#define GEARRATIO   75
#define TOTALTICKS  47
#define CONVERTVEL  (WHEELRADIUS/(GEARRATIO*TOTALTICKS))

class Motor {
private:
    u_int8_t pinA, pinB, pinE, pinEncA=0, pinEncB=1;
    bool stateA=HIGH; 
    bool stateB=HIGH;
    int  stateE=0;
    int tickChange=0;
    float velocity=0;
public:
    // constructors
    Motor(void) {
        pinA = 0;
        pinB = 1;
        pinE = 2;
        pinEncA = 11;
        pinEncB = 12;
        pinMode(pinA, OUTPUT);
        pinMode(pinB, OUTPUT);
        pinMode(pinE, OUTPUT);
    }
    Motor(int MotA, int MotB, int MotE, int EncA, int EncB) {
        pinA = MotA;
        pinB = MotB;
        pinE = MotE;
        pinEncA = EncA;
        pinEncB = EncB;
        pinMode(pinA, OUTPUT);
        pinMode(pinB, OUTPUT);
        pinMode(pinE, OUTPUT);
    }
    Motor(int MotA, int MotB, int MotE){
        pinA = 0;
        pinB = 1;
        pinE = 2;
        pinEncA = 11;
        pinEncB = 12;
        pinMode(pinA, OUTPUT);
        pinMode(pinB, OUTPUT);
        pinMode(pinE, OUTPUT);
        //Enc = enc;
    }
    int  getPinA(void) {return pinA;}
    int  getPinB(void) {return pinB;}
    int  getPinE(void) {return pinE;}
    int  getEncA(void) {return pinEncA;}
    int  getEncB(void) {return pinEncB;}
    bool getStateA(void) {return stateA;}
    bool getStateB(void) {return stateB;}
    int  getStateE(void) {return stateE;}

    float readVelocity(void) {
        tickChange = 0;
        velocity = CONVERTVEL * tickChange;
        return velocity;
    }

    void setVelocity(int speed) {
        if (speed > 0) {
            spinCW(speed);
        } else if (speed < 0) {
            spinCCW(-speed);
        } else {
            activeStop();
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
        pinEncA = M.getEncA();
        pinEncB = M.getEncB();
        stateA = M.getStateA();
        stateB = M.getStateB();
        stateE = M.getStateE();
        return *this;
    }
    ~Motor(){
    }
};

#endif 