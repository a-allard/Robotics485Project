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
    uint8_t pinA, pinB, pinE;
    volatile bool stateA = HIGH; 
    volatile bool stateB = HIGH;
    volatile int  stateE;
    volatile int tickChange = 0;
    volatile bool cw;
    volatile float measVel;
    volatile float setVel = 0;
    const int minSetVal=1;
    volatile float speed = 0;
    bool activeStopOn = true;
public:
    // constructors
    Motor(void);
    Motor(int MotA, int MotB, int MotE);
    int  getPinA(void);
    int  getPinB(void);
    int  getPinE(void);
    bool getStateA(void);
    bool getStateB(void);
    int  getStateE(void);
    float getSetVel(void);
    float getMeasVel(void);

    void updateVel(float measuredVelocity);

    int toPWM(float speed);
    void setVelocity(float vel);
    void setPWM(int speed);
    void spinCW(int speed);
    void spinCCW(int speed);
    void activeStop(void);
    void passiveStop(void);
    Motor& operator = (const Motor &);
    ~Motor();
};

#endif 