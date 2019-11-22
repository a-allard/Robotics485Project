#include <Arduino.h>
#include <Servo.h>


#define velMax 100
#define velMin 0
#define thetaMax 2*3.141596
#define thetaMin 0
#define phiMax 2*3.141596
#define phiMin 0

void updateVals();
void parseCMD();
String formVelThePhi();
String formFullCMD();

const int velServoPin=0, thetaServoPin=1, phiServoPin=2, controllerModeServoPin=3;
bool fullOutput = false;

Servo velServoSig, thetaServoSig, phiServoSig, controllerModeServoSig;
String inputString = "";
double vel = 0, theta = 0, phi = 0;
int controllerMode = 0;

IntervalTimer updateVarTimer;

void setup() {
  Serial.begin(115200);
  pinMode(velServoPin, INPUT);
  pinMode(thetaServoPin, INPUT);
  pinMode(phiServoPin, INPUT);
  pinMode(controllerModeServoPin, INPUT);
  velServoSig.attach(velServoPin);
  thetaServoSig.attach(thetaServoPin);
  phiServoSig.attach(phiServoPin);
  controllerModeServoSig.attach(controllerModeServoPin);
  updateVarTimer.begin(updateVals, 1000);
}

void loop() {
  //It all about the interrupts babe!!!
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

String formVelThePhi(){
  return String(vel) + "," + String(theta) +  "," + String(phi);
}

String formFullCMD(){
  return String(controllerMode) + "," + String(vel) + "," + String(theta) +  "," + String(phi);
}


double mapD(double val, double fromA, double fromB, double toA, double toB)
{
  return (val - fromA) * toB / fromA + toA;
}

void updateVals()
{
  Serial.println("teting");
  vel = mapD(velServoSig.read(), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, velMin, velMax);
  theta = mapD(thetaServoSig.read(), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, thetaMin, thetaMax);
  phi = mapD(phiServoSig.read(), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, phiMin, phiMax);
  controllerMode = (int)round(mapD(controllerModeServoSig.read(), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, 0, 1)) + 1;
}