// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

#include <Usb.h>
#include <Wire.h>

#define DESCRIPTOR 0
#define REPORT 1

const byte MY_ADDRESS = 9;

void setup() {
  Wire.begin(MY_ADDRESS);// join i2c bus with address #8

  TWAR = (MY_ADDRESS << 1) | 1; 
  
//  Wire.onReceive(i2cHandler); // register event
  Wire.onReceive(receiveEvent); // register event
  
  Serial.begin(115200);           // start serial for output
}

void loop() {
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
/*
 * [ HEADER [type : 1byte][len : 1 or 2 byte] ] [ DATA ]
 * 
 * type : type of DATA ( REPORT or DESCRIPTOR )
 * len : length of DATA
 * DATA : Report or Descriptor itself
 */
void receiveEvent(int howMany) {
  static uint8_t type;
  static uint16_t len, temp;
  static uint8_t *buffer;
  static boolean isHeader = true;
  
  if(isHeader){
    type = (uint8_t)Wire.read();
    len = 0;
    while(Wire.available()){
      len <<= 8;
      len += (uint8_t)Wire.read();
    }//Wire.available()
    
    temp = 0;
    buffer = (uint8_t *)malloc(len);
    isHeader = false;
  }//isHeader
  else{//Data
    while(Wire.available()){
      buffer[temp] = (uint8_t)Wire.read();
      temp++;
    }//Wire.available()
    
    if(temp >= len) {
      switch (type){
        case DESCRIPTOR:
          onDescriptor( buffer, len);
          break;
        case REPORT:
          onReport(buffer, len);
          break;
        default:
          break;
      }// switch (type)

      free(buffer);
      isHeader = true;
    }//temp >= len
  }//Data
  
}

void onDescriptor(uint8_t *buffer, uint16_t len){
  Serial.print("Descriptor \n");
  for(int i=0; i<len; i++) { 
    PrintHex(buffer[i], 0x80); 
    Serial.print(" ");
    if((i+1)%4 == 0) Serial.print("/");
    if((i+1)%16 == 0) Serial.print("\n");
  }
  Serial.print("\n");
}

void onReport(uint8_t *buffer, uint16_t len){
  Serial.print("Report \n");
  for(int i=0; i<len; i++) { PrintHex(buffer[i], 0x80); }
  Serial.print("\n");
}
