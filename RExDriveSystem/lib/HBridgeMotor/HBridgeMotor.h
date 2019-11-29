#ifndef HBRIDGEMOTOR
#define HBRIDGEMOTOR

#include <Arduino.h>
#include <Encoder.h>

class Motor {
private:
    int pinA, pinB, pinE;
    int encoderA = -1, encoderB = -1;
    bool stateA=HIGH; 
    bool stateB=HIGH;
    int  stateE=0;
    Encoder *motorEncoder;
    
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
    Motor(int A, int B, int E, int encoderA, int encoderB){
        pinA = A;
        pinB = B;
        pinE = E;
        pinMode(pinA, OUTPUT);
        pinMode(pinB, OUTPUT);
        pinMode(pinE, OUTPUT);
        this->setupEncoder(encoderA, encoderB);
    }
    ~Motor(){
        //Properly freeing memory
        delete(this->motorEncoder);
    }
    void setupEncoder(int encoderA, int encoderB){
        /*
        This sets up the encoders on the specified pins.
        Not the encoder is a pointer hence why the destructor
        was added to insure there are no memory leaks from
        dangeling pointers.
        */
        this->encoderA = encoderA;
        this->encoderB = encoderB;
        pinMode(this->encoderA, INPUT);
        pinMode(this->encoderB, INPUT);
        this->motorEncoder = new Encoder(this->encoderA, this->encoderB);
    }
    int32_t getEncoderValue(){
        //Reads the encoder value and resets it to 0 for relative readings.
        //Not sure if we want to leave it running but I think it would be better
        //to only have differences.
        /*
        Allard thought on setting up encoders:
        have them setup as part of the Hbridge class but
        because the Intervaltimer class is dum and will not 
        take in a class member function as it's callback
        the interval timer callback to read all 4 encoders probably should be in main
        or the omniwheeldrive class.  That way 1 timer reads all the wheels and keeps things running.
        This is only here to make getting the value out simpler so this will report ticks
        */
        int32_t value = this->motorEncoder->read();
        this->motorEncoder->write(0);
        return value;
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