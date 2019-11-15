#include <Arduino.h>
#include <Servo.h>

Servo s1, s2, s3;
String inputString = "";

void setup() {
  Serial.begin(115200);
  s1.read();
}

void loop() {
  // put your main code here, to run repeatedly:
}

void serialEvent(){
  while(Serial.available()){
    char c = (char)Serial.read();
    switch (c)
    {
    case '\n':
      break;
    
    default:
      inputString += String(c);
      break;
    }
    
  }
}

void parseCMD(){
  inputString = "";
  String cmd = String(inputString[0] + inputString[1] + inputString[2]);
  cmd = cmd.toLowerCase();
  if(cmd == String("vpt")){
    sendVelThePhi();
  }
}

void sendVelThePhi(){
  Serial.print("vtp");
}