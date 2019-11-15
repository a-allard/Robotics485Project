#include <Arduino.h>
#include <Servo.h>


#define velMax 100
#define velMin 0
#define thetaMax 2*3.141596
#define thetaMin 0
#define phiMax 2*3.141596
#define phiMin 0

const int velServoPin=0, thetaServoPin=1, phiServoPin=2, controllerModeServoPin=3;
bool fullOutput = false;

Servo velServoSig, thetaServoSig, phiServoSig, controllerModeServoSig;
String inputString = "";
double vel = 0, theta = 0, phi = 0;
int controllerMode = 0;



void setup() {
  Serial.begin(115200);
  pinMode(velServoPin, INPUT);
  pinMode(thetaServoPin, INPUT);
  pinMode(phiServoPin, INPUT);
  pinMode(controllerModeServoPin, INPUT);
}

void loop() {
  
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
    return;
  }
  String cmd = String(inputString[0] + inputString[1] + inputString[2]);
  cmd = cmd.toLowerCase();
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
  Serial.println(outputString);
  inputString = "";
}

String formVelThePhi(){
  return String(vel) + "," + String(theta) +  "," + String(phi);
}

String formFullCMD(){
  return String("");
}

void updateVals()
{
  vel = mapD(velServoSig.read(), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, velMin, velMax);
  theta = mapD(thetaServoSig.read(), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, thetaMin, thetaMax);
  phi = mapD(phiServoSig.read(), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, phiMin, phiMax);
  controllerMode = (int)round(mapD(controllerModeServoSig.read(), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, 0, 2));
}

double mapD(double val, double fromA, double fromB, double toA, double toB)
{
  return (val - fromA) * toB / fromA + toA;
}