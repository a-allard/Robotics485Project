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
    char c = Serial.read();
    if(c != '\n'){
      inputString += String(c);
    }
    else
    {
      break;
    }
    
  }
}

void parseCMD(){

  inputString = "";
}