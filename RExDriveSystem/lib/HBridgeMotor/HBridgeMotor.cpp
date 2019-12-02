#include <HBridgeMotor.h>

Motor::Motor(void) {
        pinA = 0;
        pinB = 1;
        pinE = 2;
        pinMode(pinA, OUTPUT);
        pinMode(pinB, OUTPUT);
        pinMode(pinE, OUTPUT);
    }
    Motor::Motor(int MotA, int MotB, int MotE) {
        pinA = MotA;
        pinB = MotB;
        pinE = MotE;
        pinMode(pinA, OUTPUT);
        pinMode(pinB, OUTPUT);
        pinMode(pinE, OUTPUT);
    }
    int  Motor::getPinA(void) {return this->pinA;}
    int  Motor::getPinB(void) {return this->pinB;}
    int  Motor::getPinE(void) {return this->pinE;}
    bool Motor::getStateA(void) {return this->stateA;}
    bool Motor::getStateB(void) {return this->stateB;}
    int  Motor::getStateE(void) {return this->stateE;}
    float Motor::getSetVel(void) {return this->setVel;}
    float Motor::getMeasVel(void) {return this->measVel;}

    void Motor::updateVel(float measuredVelocity){
        if(!this->activeStopOn){
            this->measVel = measuredVelocity / 0.03; // wheel rotations per second.
            float diff = -(this->setVel - this->measVel);
            float newSpeed = speed;
            newSpeed += diff * 20;
            setPWM(toPWM(newSpeed));
        }
    }

    int Motor::toPWM(float speed){
        this->speed = speed;
        int pwm = 0;
        pwm = (int)round(speed * 1);
        return pwm;
    }
    void Motor::setVelocity(float vel){
        this->setVel = vel / 0.03;
        this->activeStopOn = false;
        if(this->setVel == 0){
            this->activeStop();
        }
    }
    void Motor::setPWM(int speed) {
        if(!this->activeStopOn){
            if(abs(speed) > 1){
            if (speed > 0) {
                spinCW(min(speed, 255));
            } else if (speed < 0) {
                spinCCW(min(-speed, 255));
            } else
                activeStop();
            }
            else {
                //spinCW(0);
            }
        }
    }
    void Motor::spinCW(int speed) {
        this->cw=true;
        stateA = HIGH;
        stateB = LOW;
        stateE = speed;
        digitalWrite(pinA, stateA);
        digitalWrite(pinB, stateB);
        analogWrite(pinE, stateE);
    }
    void Motor::spinCCW(int speed) {
        this->cw=false;
        stateA = LOW;
        stateB = HIGH;
        stateE = speed;
        digitalWrite(pinA, stateA);
        digitalWrite(pinB, stateB);
        analogWrite(pinE, stateE);
    }
    void Motor::activeStop(void) {
        this->activeStopOn=true;
        stateA = HIGH;
        stateB = HIGH;
        stateE = 0;
        this->speed = 0;
        this->setVel = 0;
        digitalWrite(pinA, stateA);
        digitalWrite(pinB, stateB);
        analogWrite(pinE, stateE);

    }
    void Motor::passiveStop(void) {
        this->activeStopOn=true;
        stateA = HIGH;
        stateB = HIGH;
        stateE = 255;
        this->speed = 0;
        this->setVel = 0;
        digitalWrite(pinA, stateA);
        digitalWrite(pinB, stateB);
        analogWrite(pinE, stateE);
    }
    Motor& Motor::operator = (const Motor &M) {
        this->pinA = M.pinA;
        this->pinB = M.pinB;
        this->pinE = M.pinE;
        this->stateA = M.stateA;
        this->stateB = M.stateB;
        this->stateE = M.stateE;
        return *this;
    }
    Motor::~Motor(){
    }