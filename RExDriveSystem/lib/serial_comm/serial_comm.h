#ifndef SERIALCOMM
#define SERIALCOMM

#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

// Function Prototypes
String getSerialMsg(void);
void parseMsg(float *cmd, String msg);

// Defined Constants
enum Commands {
    STP,
    LFS, 
    LFM, 
    VEL,
    VELQUERY, 
    VELSET
};

String getSerialMsg(void) {
  // no messages should be longer than 64 characters
  static String message;
  String temp;
  char buffer;
  //for (int i = 0; i < 64; i++) {
  //  while(!Serial.available()); // wait until something is sent
    buffer = Serial.read();
    // Serial.print(buffer);
    if (buffer == '\n') {
      temp = message;
      message = "";
      return temp;
    } else {
      message += buffer;
      return '0';
    }
}

void parseMsg(float *cmd, String msg) {
  // grab the 1st three characters that define the command
  msg.toUpperCase();
  String command = msg.substring(0, 3); // between characters 0 and 2
  if (0 == command.compareTo("STP")) {
    cmd[0] = STP;
  } else 
  if (0 == command.compareTo("LFS")) {
    cmd[0] = LFS;
  } else 
  if (0 == command.compareTo("LFM")) {
    cmd[0] = LFM;
  } else 
  if (0 == command.compareTo("VEL")) {
    cmd[0] = VEL;
  } else {
    cmd[0] = -1; 
  }

  // determine the arguments that are expected after the command
  String arguments, arg1, arg2, arg3;
  switch ( (int)cmd[0]) {
    case STP:
      // No arguments for Stop all Motion
      break;
    
    case LFS:
      // No arguments for Line Following Status
      break;
    
    case LFM:
      // 1 argument for Line Following Mode
      // type: boolean
      cmd[1] =  msg.charAt(4) - 48; // convert char to decimal
      if ((cmd[1] != 0) && (cmd[1] != 1)) {
        cmd[0] = -1;
      }
      break;
    
    case VEL:
      // 2 commands linked to this keyword, differ by the 1st argument
      if (msg.charAt(4) == '?') {cmd[0] = VELQUERY; break;}
      cmd[0] = VELSET;
      arguments = msg.substring(4); // everything after character 4
      
      int comma = arguments.indexOf(',');
      arg1 = arguments.substring(0, comma);
      cmd[1] = arg1.toFloat();
      arguments = arguments.substring(comma + 1);
      
      comma = arguments.indexOf(',');
      arg2 = arguments.substring(0, comma);
      cmd[2] = arg2.toFloat();
      arguments = arguments.substring(comma + 1);

      arg3 = arguments;
      cmd[3] = arg3.toFloat();
      break;
  }
}

#endif 