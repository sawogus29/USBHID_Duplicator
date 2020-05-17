// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

#include <Usb.h>
#include <Wire.h>
#include <HID.h>
#include <avr/wdt.h>
#include <EEPROM.h>

#define DESCRIPTOR 0
#define REPORT 1

const byte MY_ADDRESS = 9;

union{
  uint16_t _16;
  uint8_t _8[2];
} uni_len;

uint8_t descriptor[54] = {
  0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x05, 0x08, 0x19, 0x01, 0x29, 0x03, 0x15, 0x00, 0x25, 0x01,
0x75, 0x01, 0x95, 0x03, 0x91, 0x02, 0x95, 0x05, 0x91, 0x01, 0x05, 0x07, 0x19, 0xE0, 0x29, 0xE7,
0x95, 0x08, 0x81, 0x02, 0x75, 0x08, 0x95, 0x01, 0x81, 0x01, 0x19, 0x00, 0x29, 0x91, 0x26, 0xFF,
0x00, 0x95, 0x06, 0x81, 0x00, 0xC0
};

void setup() {
  Serial.begin(115200);           // start serial for output
  
//  uni_len._8[0] = (uint8_t)EEPROM.read(0);
//  uni_len._8[1] = (uint8_t)EEPROM.read(1);
//  static uint16_t len = uni_len._16;
//  static uint8_t *buffer;
//  if(len > 128){
//    Serial.println("Descriptor Length is too long");
//  }else{
//    buffer = (uint8_t *)malloc(len);
//    for(int i=0; i<len; i++){
//      buffer[i] = (uint8_t)EEPROM.read(i+2);
//    }
//    static HIDSubDescriptor node(buffer, len);
//    HID().AppendDescriptor(&node);
//  }
  static HIDSubDescriptor node(descriptor, 54);
  HID().AppendDescriptor(&node);
  
  Wire.begin(MY_ADDRESS);// join i2c bus with address #8
  TWAR = (MY_ADDRESS << 1) | 1; // enable broadcast receive  
  Wire.onReceive(receiveEvent); // register event
}

void loop() {
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
/*
 * [ HEADER [type : 1byte][len : 2 byte] ] [ DATA ]
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
  /*****Edit Here********/
  Serial.print("Descriptor \n");
  for(int i=0; i<len; i++) { PrintHex(buffer[i], 0x80); Serial.print(" "); if((i+1)%4 == 0) Serial.print("/"); if((i+1)%16 == 0) Serial.print("\n");}
  Serial.print("\n");

  uni_len._16 = len;
  EEPROM.write(0,(uint8_t)(uni_len._8[0]));
  EEPROM.write(1,(uint8_t)(uni_len._8[1]));
  for(int i = 0; i < len; i++){
    EEPROM.write(2+i, buffer[i]);
  }

  //Restart
  wdt_enable(WDTO_8S);
  while(1)
  {
    Serial.println("restart...");
    delay(1000);
//     wdt_reset();          // uncomment to avoid reboot
  }
  /************************/
}


void onReport(uint8_t *buffer, uint16_t len){
  /*****Edit Here********/
  Serial.print("Report \n");
  for(int i=0; i<len; i++) { PrintHex(buffer[i], 0x80); }
  Serial.print("\n");

  HID().SendReport(2, buffer, len);
  /************************/
}
