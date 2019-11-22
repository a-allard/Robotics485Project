#include "HBridge.h"

L298n::L298n(uint8_t inApin, uint8_t inBpin, uint8_t enablePin){
    this->hBridgeAPin=inApin;
    this->hBridgeBPin=inBpin;
    this->hBridgeEnablePin=enablePin;
    pinMode(this->hBridgeAPin, OUTPUT);
    pinMode(this->hBridgeBPin, OUTPUT);
    pinMode(this->hBridgeEnablePin, OUTPUT);
    
}
void L298n::setSpeed(int speed){
    if(speed<0)
    {
        this->setDirection(!this->forward);
        speed *= -1;
    }
    if(speed>255){
        speed=255;
    }
    this->speed=speed;
}
void L298n::setDirection(bool ccw=false, bool seconCall){
    if(!(seconCall)){
        if(this->directionFlipped){
            this->setDirection(!ccw, true);
            return;
        }
    }

    if(ccw){
        digitalWriteFast(this->hBridgeAPin, HIGH);
        digitalWriteFast(this->hBridgeBPin, LOW);
        this->forward=true;
    }
    else{
        digitalWriteFast(this->hBridgeAPin, LOW);
        digitalWriteFast(this->hBridgeBPin, HIGH);
        this->forward=false;
    }
}
void L298n::setupEncoders(uint8_t aPin, uint8_t bPin, bool inverted){
    this->encoderAPin=aPin;
    this->encoderBPin=bPin;
    this->encoderDirectionFlipped=inverted;
    pinMode(this->encoderAPin, INPUT);
    pinMode(this->encoderBPin, INPUT);
    this->motorEncoder = new Encoder(this->encoderAPin, this->encoderBPin);
    //this->encoderTimer.begin(this->timerCallbackReadEncoder, 10000);
}
int L298n::getSpeedSetting(){
    return this->speed;
}
float L298n::getSpeed(){
    return round(this->speedReading);
}
void L298n::setFlippedDirection(bool flipped){
    this->directionFlipped=flipped;
}
void L298n::start(){
    analogWrite(this->hBridgeEnablePin, this->speed);
}
void L298n::stop(){
    digitalWriteFast(this->hBridgeEnablePin, LOW);
}

void L298n::timerCallbackReadEncoder(){
    int32_t counts = this->motorEncoder->read();
    this->speedReading = (float)counts / (float)this->countsPerRev / (float)this->gearBox / 0.01 * 60;
}