#include <Arduino.h>

#define MIN_PULSE_WIDTH 990
#define MAX_PULSE_WIDTH 2013


#define velMax 100
#define velMin 0
#define thetaMax 100
#define thetaMin 0
#define phiMax 100
#define phiMin 0


const int velServoPin=2, thetaServoPin=1, phiServoPin=3, controllerModeServoPin=4;
bool fullOutput = false;

String inputString = "";
double vel = 0, theta = 0, phi = 0;
int controllerMode = 0;

IntervalTimer updateVarTimer;



void loop() {
  //It all about the interrupts babe!!!
}



String formVelThePhi(){
  return String(vel) + "," + String(theta) +  "," + String(phi);
}

String formFullCMD(){
  return String(controllerMode) + "," + String(vel) + "," + String(theta) +  "," + String(phi);
}


void parseCMD(){
  if(inputString.length() < 3){
    Serial.println("ERROR: Invalid input string");
    inputString = "";
    return;
  }
  inputString.trim();
  inputString.replace(" ", "");
  inputString.toLowerCase();
  String cmd = String(inputString[0]) + String(inputString[1]) + String(inputString[2]);
  String outputString = "";
  if(fullOutput){
    outputString += cmd + String(" = ");
  }
  if(cmd == String("vpt")){
    outputString += formVelThePhi();
  }
  if(cmd == String("bcm")){
    outputString += String(controllerMode);
  }
  if(cmd == String("fcs")){
    outputString += formFullCMD();
  }
  if(cmd == String("fco")){
    if(inputString.indexOf("=") > 0){
      fullOutput = (bool)String(inputString[inputString[inputString.indexOf("=") + 1]]).toInt();
      
    }
  }
  Serial.println(outputString);
  inputString = "";
}

void serialEvent(){
  while(Serial.available()){
    char c = (char)Serial.read();
    if(fullOutput){
      Serial.print(c);
    }
    switch (c)
    {
    case '\n':
      if(fullOutput){
        //Serial.println(inputString);
      }
      parseCMD();
      break;
    
    default:
      inputString += String(c);
      break;
    }
    
  }
}


double mapD(double val, double fromA, double fromB, double toA, double toB)
{
  return (val - fromA) * toB / fromA + toA;
}



void updateVals()
{
  vel = (mapD(pulseIn(velServoPin, HIGH), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, velMin, velMax) - 50) * 2;
  theta = (mapD(pulseIn(thetaServoPin, HIGH), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, thetaMin, thetaMax) - 50) * 2;
  phi = (mapD(pulseIn(phiServoPin, HIGH), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, phiMin, phiMax) - 50) * 2;
  controllerMode = (int)round(mapD(pulseIn(controllerModeServoPin, HIGH), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, 0, 2));
}


void setup() {
  Serial.begin(115200);
  pinMode(velServoPin, INPUT);
  pinMode(thetaServoPin, INPUT);
  pinMode(phiServoPin, INPUT);
  pinMode(controllerModeServoPin, INPUT);
  // velServoSig.attach(velServoPin);
  // thetaServoSig.attach(thetaServoPin);
  // phiServoSig.attach(phiServoPin);
  // controllerModeServoSig.attach(controllerModeServoPin);
  updateVarTimer.begin(updateVals, 80000);
}