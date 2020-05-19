//#define __DEBUG__

#include <Usb.h>
#include <Wire.h>
#include <MyHID.h>
#include <avr/wdt.h>
#include <EEPROM.h>

#define DESCRIPTOR 0
#define REPORT 1
static const uint8_t _EEPROM_MAGIC = 0x99;
const byte MY_ADDRESS = 9;

union{
  uint16_t _16;
  uint8_t _8[2];
} uni_len;

static uint8_t *_hidReportDescriptor;
static boolean _isValidEEPROM = false;

void setup() {
  int index = 0;
  if( _isValidEEPROM = (EEPROM.read(index++) == _EEPROM_MAGIC) ){
    uni_len._8[0] = (uint8_t)EEPROM.read(index++);
    uni_len._8[1] = (uint8_t)EEPROM.read(index++);
    static uint16_t len = uni_len._16;
    _hidReportDescriptor = (uint8_t *)malloc(len);
    for(int i=0; i<len; i++, index++){
      _hidReportDescriptor[i] = (uint8_t)EEPROM.read(index);
    }
    static HIDSubDescriptor node(_hidReportDescriptor, len);
    HID().AppendDescriptor(&node);
  }
  
  Wire.begin(MY_ADDRESS);// join i2c bus with address #8
  TWAR = (MY_ADDRESS << 1) | 1; // enable broadcast receive  
  Wire.onReceive(receiveEvent); // register event

  Serial.begin(115200);           // start serial for output
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

void reboot(){
  if(Serial) { Serial.end(); }
  
  wdt_enable(WDTO_15MS);     // enable the watchdog
  while(1){}
}

void onDescriptor(uint8_t *buffer, uint16_t len){
  /*****Edit Here********/
#ifdef __DEBUG__
  Serial.print("Descriptor \n");
  Serial.print("len : "); Serial.println(len);
  for(int i=0; i<len; i++) { PrintHex(buffer[i], 0x80); Serial.print(" "); if((i+1)%4 == 0) Serial.print("/"); if((i+1)%16 == 0) Serial.print("\n");}
  Serial.print("\n");
#endif
  int index = 0;
  EEPROM.write(index++, _EEPROM_MAGIC);
  uni_len._16 = len;
  EEPROM.write(index++,(uint8_t)(uni_len._8[0]));
  EEPROM.write(index++,(uint8_t)(uni_len._8[1]));
  for(int i = 0; i < len; i++, index++){
    EEPROM.write(index, buffer[i]);
  }

  //Ensure EEPROM writing completed
  while(buffer[len-1] != EEPROM.read(index-1)){
#ifdef __DEBUG__
    Serial.println("EEPROM writing not complete");  
#endif
  }

  reboot();
  /************************/
}


void onReport(uint8_t *buffer, uint16_t len){
  /*****Edit Here********/
#ifdef __DEBUG__
  Serial.print("Report \n");
  for(int i=0; i<len; i++) { PrintHex(buffer[i], 0x80); }
  Serial.print("\n");
#endif
  if(_isValidEEPROM){
    HID().SendReport(0/*unused*/, buffer, len);
  }
  /************************/
}
