/*========================================================================== 
// Author : Handson Technology 
// Project : Arduino Uno 
// Description : L298N Motor Driver 
// Source-Code : L298N_Motor.ino 
// Program: Control 2 DC motors using L298N H Bridge Driver 
//========================================================================== */ 
#include <Arduino.h>

// Definitions Arduino pins connected to input H Bridge 
#define RightE  7
#define RightB  8
#define RightA  9
#define LeftA   10
#define LeftB   11
#define LeftE   12
#define BackE   13
#define BackB   14
#define BackA   15
#define FrontA  16
#define FrontB  17
#define FrontE  18

#define start 0
#define end 255

void bodyFwd(int speed);
void bodyRev(int speed);
void bodyLeft(int speed);
void bodyRight(int speed);
void frontCW(int speed);
void frontCCW(int speed);
void backCW(int speed);
void backCCW(int speed);
void leftCW(int speed);
void leftCCW(int speed);
void rightCW(int speed);
void rightCCW(int speed);

void setup() {
  // Set the output pins
  pinMode(RightE, OUTPUT);
  pinMode(RightB, OUTPUT);
  pinMode(RightA, OUTPUT);
  pinMode(LeftB,  OUTPUT);
  pinMode(LeftA,  OUTPUT);
  pinMode(LeftE,  OUTPUT);
  pinMode(BackE,  OUTPUT);
  pinMode(BackB,  OUTPUT);
  pinMode(BackA,  OUTPUT);
  pinMode(FrontB, OUTPUT);
  pinMode(FrontA, OUTPUT);
  pinMode(FrontE, OUTPUT);
}

void loop() {
  // bodyFwd & stop
  for (int i = start; i < end; i++) {
    bodyFwd(i);
    delay(10);
  }
  for (int i = end; i > start; i--) {
    bodyFwd(i);
    delay(10);
  }
  
  // bodyRev & stop
  for (int i = start; i < end; i++) {
    bodyRev(i);
    delay(10);
  }
  for (int i = end; i > start; i--) {
    bodyRev(i);
    delay(10);
  }
  
  // bodyLeft & stop
  for (int i = start; i < end; i++) {
    bodyLeft(i);
    delay(10);
  }
  for (int i = end; i > start; i--) {
    bodyLeft(i);
    delay(10);
  }
  
  // bodyRight & stop
  for (int i = start; i < end; i++) {
    bodyRight(i);
    delay(10);
  }
  for (int i = end; i > start; i--) {
    bodyRight(i);
    delay(10);
  }
}

void bodyFwd(int speed) {
  leftCCW(speed);
  rightCW(speed);
  return;
}

void bodyRev(int speed) {
  leftCW(speed);
  rightCCW(speed);
  return;
}

void bodyLeft(int speed) {
  frontCW(speed);
  backCCW(speed);
  return;
}

void bodyRight(int speed) {
  frontCCW(speed);
  backCW(speed);
  return;
}

void frontCW(int speed) {
  digitalWrite(FrontA, HIGH); 
  digitalWrite(FrontB, LOW); 
  analogWrite(FrontE, speed);
  return;
}

void frontCCW(int speed) {
  digitalWrite(FrontA, LOW); 
  digitalWrite(FrontB, HIGH); 
  analogWrite(FrontE, speed);
  return;
}

void backCW(int speed) {
  digitalWrite(BackA, HIGH); 
  digitalWrite(BackB, LOW); 
  analogWrite(BackE, speed);
  return;
}

void backCCW(int speed) {
  digitalWrite(BackA, LOW); 
  digitalWrite(BackB, HIGH); 
  analogWrite(BackE, speed);
  return;
}

void leftCW(int speed) {
  digitalWrite(LeftA, HIGH); 
  digitalWrite(LeftB, LOW); 
  analogWrite(LeftE, speed);
  return;
}

void leftCCW(int speed) {
  digitalWrite(LeftA, LOW); 
  digitalWrite(LeftB, HIGH); 
  analogWrite(LeftE, speed);
  return;
}

void rightCW(int speed) {
  digitalWrite(RightA, HIGH); 
  digitalWrite(RightB, LOW); 
  analogWrite(RightE, speed);
  return;
}

void rightCCW(int speed) {
  digitalWrite(RightA, LOW); 
  digitalWrite(RightB, HIGH); 
  analogWrite(RightE, speed);
  return;
}

