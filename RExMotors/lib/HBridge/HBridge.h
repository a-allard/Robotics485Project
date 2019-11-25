#ifndef __HBridge__
#define __HBridge__
#include "Arduino.h"
#include <Encoder.h>
#include <IntervalTimer.h>

class L298n
{
    private:
    const int gearBox=75, countsPerRev=12;
    uint8_t encoderAPin, encoderBPin, hBridgeAPin, hBridgeBPin, hBridgeEnablePin;
    bool encoderDirectionFlipped=false;
    bool forward=true;
    int speed=0;
    float speedReading=0;  //RPMs
    bool directionFlipped=false;
    Encoder *motorEncoder;

    public:
    L298n(uint8_t inApin, uint8_t inBpin, uint8_t enablePin);
    void setSpeed(int speed);
    void setDirection(bool ccw=false, bool seconCall=false);
    void setupEncoders(uint8_t aPin, uint8_t bPin, bool inverted=false);
    int getSpeedSetting();
    float getSpeed();
    void setFlippedDirection(bool flipped=false);
    void start();
    void stop();
    void timerCallbackReadEncoder(double time);
};

#endif
