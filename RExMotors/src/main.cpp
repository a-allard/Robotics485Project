#include <Arduino.h>
#include "../lib/HBridge/HBridge.h"

// The many... many... pin declarations.
#define frontMotorEncoderA 13
#define frontMotorEncoderB 14
#define frontHBridgeA 9
#define frontHBridgeB 10
#define frontHBridgeEnable 11

#define backMotorEncoderA 15
#define backMotorEncoderB 16
#define backHBridgeA 6
#define backHBridgeB 7
#define backHBridgeEnable 8

#define leftMotorEncoderA 20
#define leftMotorEncoderB 21
#define leftHBridgeA 1
#define leftHBridgeB 2
#define leftHBridgeEnable 0

#define rightMotorEncoderA 22
#define rightMotorEncoderB 23
#define rightHBridgeA 3
#define rightHBridgeB 4
#define rightHBridgeEnable 5

#define leftIRPin 21
#define rightIRPin 22


//IR vars
const int irThreshold = 1100, maxIRReadTime = 2000;

// motors
L298n frontMotor(frontHBridgeA, frontHBridgeB, frontHBridgeEnable);
L298n backMotor(backHBridgeA, backHBridgeB, backHBridgeEnable);
L298n leftMotor(leftHBridgeA, leftHBridgeB, leftHBridgeEnable);
L298n rightMotor(rightHBridgeA, rightHBridgeB, rightHBridgeEnable);

//Serial and communication crap
bool fullOutput = false;
String inputString = "";
float vel = 0, theta = 0, phi = 0;
bool followLineMode = true;

String returnVelData();
void parseCMD();
void followLine();
void forward(int speed);
void stopMotors();
bool rightIROnLine();
bool leftIROnLine();
void parseVelCommand(String command);
void startDrivePolar();

void setup() {
  Serial.begin(115200);
  frontMotor.setupEncoders(frontMotorEncoderA, frontMotorEncoderB);
  backMotor.setupEncoders(backMotorEncoderA, backMotorEncoderB);
  leftMotor.setupEncoders(leftMotorEncoderA, leftMotorEncoderB);
  rightMotor.setupEncoders(rightMotorEncoderA, rightMotorEncoderB);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(followLineMode){
    followLine();
  }
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
  if(cmd == String("vel")){
    if(inputString.indexOf("?") > 0){
      outputString += returnVelData();
    }
    else{
      parseVelCommand(inputString.substring(3));
    }
    
    outputString += "";
  }
  if(cmd == String("lfm")){
    if(inputString.indexOf("=") > 0){
      followLineMode = (bool)String(inputString[inputString[inputString.indexOf("=") + 1]]).toInt();
    }
    else{
      outputString += String(followLineMode);
    }
  }
  if(cmd == String("lfs")){
    outputString += String(followLineMode);
  }
  if(cmd == String("stp")){
    stopMotors();
    outputString += "Done";
  }
  if(cmd == String("fco")){
    if(inputString.indexOf("=") > 0){
      fullOutput = (bool)String(inputString[inputString[inputString.indexOf("=") + 1]]).toInt();
    }
    else{
      outputString += String(fullOutput);
    }
  }
  
  Serial.println(outputString);
  inputString = "";
}

String returnVelData(){
  return String(vel) + "," + String(theta) +  "," + String(phi);
}

/*
For line following things are going to be a bit simplified
We will only use the left and right wheels
*/

void followLine(){
  stopMotors();
  //Find line with middle reflective sensor
  //If there is no line with the middle sensor this will exit.
  int maxCounts=500;
  //drive forward.
  forward(90);
  int timeCounter=0;
  while(1){
    if(!followLineMode){
      stopMotors();
      return;
    }
    while(leftIROnLine()){
      leftMotor.stop();
      delay(10);
      timeCounter++;
      if(timeCounter>maxCounts){
        followLineMode=false;
        return;
      }
    }
    timeCounter=0;
    leftMotor.start();
    while(rightIROnLine()){
      rightMotor.stop();
      delay(10);
      timeCounter++;
      if(timeCounter>maxCounts){
        followLineMode=false;
        return;
      }
    }
    timeCounter=0;
    rightMotor.start();
  }
}

void forward(int speed){
  leftMotor.setDirection(false);
  rightMotor.setDirection(true);
  leftMotor.setSpeed(speed);
  rightMotor.setSpeed(speed);
  leftMotor.start();
  rightMotor.start();
}
void stopMotors(){
  frontMotor.stop();
  backMotor.stop();
  leftMotor.stop();
  rightMotor.stop();
  vel=0;
  theta=0;
  phi=0;
}

bool leftIROnLine(){
  bool onLine=false;
  pinMode(leftIRPin, OUTPUT);
  digitalWriteFast(leftIRPin, HIGH);
  delayMicroseconds(10);
  pinMode(leftIRPin, INPUT);
  long readTime = micros();
  while(digitalReadFast(leftIRPin) == HIGH && micros() - readTime < maxIRReadTime);
  int time = micros() - readTime;
  if(time > irThreshold){
    onLine = true;
  }
  return onLine;
}
bool rightIROnLine(){
  bool onLine=false;
  pinMode(rightIRPin, OUTPUT);
  digitalWriteFast(rightIRPin, HIGH);
  delayMicroseconds(10);
  pinMode(rightIRPin, INPUT);
  long readTime = micros();
  while(digitalReadFast(rightIRPin) == HIGH && micros() - readTime < maxIRReadTime);
  int time = micros() - readTime;
  if(time > irThreshold){
    onLine = true;
  }
  return onLine;
}

void parseVelCommand(String command){
  vel=0;
  theta=0;
  phi=0;
  String velCommand = command.substring(0, command.indexOf(","));
  command = command.substring(command.indexOf(",") + 1);
  String thetaCommand = command.substring(0, command.indexOf(","));
  String phiCommand = command.substring(command.indexOf(",") + 1);
  vel = velCommand.toFloat();
  theta = thetaCommand.toFloat();
  phi = phiCommand.toFloat();
  startDrivePolar();
}

void startDrivePolar(){
  frontMotor.setSpeed(vel * -1 * sin(theta + phi));
  backMotor.setSpeed(vel * sin(theta + phi));
  leftMotor.setSpeed(vel * cos(theta + phi));
  rightMotor.setSpeed(vel * -1 * cos(theta + phi));

  frontMotor.start();
  backMotor.start();
  leftMotor.start();
  rightMotor.start();
}