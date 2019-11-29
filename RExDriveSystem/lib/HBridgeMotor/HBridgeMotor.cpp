#include <HBridgeMotor.h>


Motor::Motor(int A, int B, int E, int encoderA, int encoderB){
        pinA = A;
        pinB = B;
        pinE = E;
        pinMode(pinA, OUTPUT);
        pinMode(pinB, OUTPUT);
        pinMode(pinE, OUTPUT);
        this->setupEncoder(encoderA, encoderB);
    }
Motor::Motor(void) {
        pinA = 0;
        pinB = 1;
        pinE = 2;
        pinMode(pinA, OUTPUT);
        pinMode(pinB, OUTPUT);
        pinMode(pinE, OUTPUT);
    }
Motor::Motor(int A, int B, int E) {
        pinA = A;
        pinB = B;
        pinE = E;
        pinMode(pinA, OUTPUT);
        pinMode(pinB, OUTPUT);
        pinMode(pinE, OUTPUT);
    }
Motor::~Motor(){
        //Properly freeing memory
        delete(this->motorEncoder);
    }
    void Motor::setupEncoder(int encoderA, int encoderB){
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
        //this->motorEncoder = new Encoder(this->encoderA, this->encoderB);
    }
    int32_t Motor::getEncoderValue(){
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
        int32_t value = this->motorEncoder->readAndReset();
        return value;
    }