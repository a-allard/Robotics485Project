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
    u_int8_t pinA, pinB, pinE;
    bool stateA=HIGH; 
    bool stateB=HIGH;
    int  stateE=0;
    int tickChange=0;
    float setVel=0, measVel = 0;
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
    Motor(int MotA, int MotB, int MotE) {
        pinA = MotA;
        pinB = MotB;
        pinE = MotE;
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

    void updateVel(float measuredVelocity){
        measVel = measuredVelocity * 75 / 0.03; // motor shaft rotations per second.
        float diff = this->setVel - measVel;
        //Serial.println(setVel);
        setPWM(toPWM((diff)) + stateE);
    }

    int toPWM(float speed){
        int pwm = 0;
        pwm = (int)round(speed * 0.5);
        return pwm;
    }
    void setVelocity(float vel){
        this->setVel = vel * 75 / 0.03;
        setPWM(toPWM((this->setVel - measVel)) + stateE);
        //Serial.println(setVel);
    }
    void setPWM(int speed) {
        //Serial.println(this->setVel);
        if (speed > 0) {
            spinCW(min(speed, 255));
        } else if (speed < 0) {
            spinCCW(min(-speed, 255));
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
        this->setVel = 0;
        digitalWrite(pinA, stateA);
        digitalWrite(pinB, stateB);
        analogWrite(pinE, stateE);
    }
    void passiveStop(void) {
        stateA = HIGH;
        stateB = HIGH;
        stateE = 255;
        this->setVel = 0;
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
    ~Motor(){
    }
};

#endif 