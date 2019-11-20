#include <Arduino.h>
#include "../lib/HBridge/HBridge.h"

#define frontMotorEncoderA 0
#define frontMotorEncoderB 1
#define frontHBridgeA 2
#define frontHBridgeB 3
#define frontHBridgeEnable 4

#define backMotorEncoderA 0
#define backMotorEncoderB 1
#define backHBridgeA 2
#define backHBridgeB 3
#define backHBridgeEnable 4

#define leftMotorEncoderA 0
#define leftMotorEncoderB 1
#define leftHBridgeA 2
#define leftHBridgeB 3
#define leftHBridgeEnable 4

#define rightMotorEncoderA 0
#define rightMotorEncoderB 1
#define rightHBridgeA 2
#define rightHBridgeB 3
#define rightHBridgeEnable 4


L298n frontMotor(frontHBridgeA, frontHBridgeB, frontHBridgeEnable);
L298n backMotor(backHBridgeA, backHBridgeB, backHBridgeEnable);
L298n leftMotor(leftHBridgeA, leftHBridgeB, leftHBridgeEnable);
L298n rightMotor(rightHBridgeA, rightHBridgeB, rightHBridgeEnable);

//Serial crap
bool fullOutput = false;
String inputString = "";
float vel = 0, theta = 0, phi = 0;



void setup() {
  Serial.begin(115200);
  frontMotor.setupEncoders(frontMotorEncoderA, frontMotorEncoderB);
  backMotor.setupEncoders(backMotorEncoderA, backMotorEncoderB);
  leftMotor.setupEncoders(leftMotorEncoderA, leftMotorEncoderB);
  rightMotor.setupEncoders(rightMotorEncoderA, rightMotorEncoderB);
}

void loop() {
  // put your main code here, to run repeatedly:
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
  inputString.trim();
  inputString.replace(" ", "");
  inputString.toLowerCase();
  String cmd = String(inputString[0] + inputString[1] + inputString[2]);
  String outputString = "";
  if(fullOutput){
    outputString += cmd + String(" = ");
  }
  if(cmd == String("vel")){
    if(inputString.indexOf("?") > 0){
      outputString += returnVelData();
    }
    else{

    }
    
    outputString += "";
  }
  if(cmd == String("fco")){
    if(inputString.indexOf("=") > 0){
      fullOutput = (bool)String(inputString[inputString[inputString.indexOf("=") + 1]]).toInt();
      
    }
  }
  Serial.println(outputString);
  inputString = "";
}

String returnVelData(){
  return String(vel) + "," + String(theta) +  "," + String(phi);
}