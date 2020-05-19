//#define __DEBUG__

#include <usbhid.h>
#include <hiduniversal.h>
#include <hidescriptorparser.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

#include <Wire.h>
#define DESCRIPTOR 0
#define REPORT 1

void mySend(byte type, uint16_t len, uint8_t *buf){
    static const int WIRE_BUFFER_SIZE = 32;
  
    //HEADER
    Wire.beginTransmission(0); // broadcast to all slave
    Wire.write(type);
    Wire.write(0);
    Wire.write(len);
    Wire.endTransmission();    // stop transmitting
    
    //DATA
    uint16_t temp = 0;
    while( temp < len ){
      Wire.beginTransmission(0); // broadcast to all slave
      Wire.write(buf+temp, ((len - temp) < WIRE_BUFFER_SIZE) ? (len - temp) : WIRE_BUFFER_SIZE);
      Wire.endTransmission();    // stop transmitting
      temp += WIRE_BUFFER_SIZE;
      delay(100);
    }
}

class MyReportParser : public HIDReportParser
{
  const byte type = REPORT;
  void Parse(USBHID *hid __attribute__((unused)), bool is_rpt_id __attribute__((unused)), uint8_t len, uint8_t *buf) {
#ifdef __DEBUG__
    Serial.println("Report");
    for(uint16_t i = 0; i < len ; i++){
      PrintHex(buf[i], 0x80);
      Serial.print(" ");
    }
    Serial.print("\n");
#endif
    mySend(type, (uint16_t)len, buf);
  }
};

class MyReportDescParser : public USBReadParser
{
  const byte type = DESCRIPTOR;
  const uint8_t USB_HOST_SHIELD_constBufLen = 64; // Defined in USB_Host_Shield/usbhid.cpp - USBHID::GetReportDesc()
  static const uint8_t REPORT_DESC_MAX_LEN = 128;
  uint8_t reportDesc[REPORT_DESC_MAX_LEN];
  
  void Parse(const uint16_t len, const uint8_t *pbuf, const uint16_t &offset) {
    memcpy(reportDesc + offset, pbuf, len);
    
    if( len < USB_HOST_SHIELD_constBufLen ){
      uint16_t reportDescLen = offset + len;
      
#ifdef __DEBUG__
      Serial.println("Descriptor");
      Serial.print("len : ");Serial.println(reportDescLen);
      for(uint16_t i = 0; i < reportDescLen ; i++){
        PrintHex(reportDesc[i], 0x80);
        Serial.print(" ");
        if((i+1)%4 == 0) Serial.print("/");
        if((i+1)%16 == 0) Serial.print("\n");
      }
      Serial.print("\n");
#endif

      mySend(type, reportDescLen, reportDesc);
    }
      
   }
};

class MyHID : public HIDUniversal
{
public:
    MyHID(USB *usb) : HIDUniversal(usb) {};

protected:
    uint8_t OnInitSuccessful(){
        uint8_t    rcode;
        MyReportDescParser                                myReportDescriptorParser;  //ReportDescriptorParser => USBReadParser
    
        if ((rcode = GetReportDescr(0, &myReportDescriptorParser)))
            goto FailGetReportDescr1;
    
        return 0;
    
    FailGetReportDescr1:
        USBTRACE("GetReportDescr1:");
        goto Fail;
    
    Fail:
        Serial.println(rcode, HEX);
        Release();
        return rcode;
    }
};


USB     Usb;
USBHub     Hub(&Usb);
MyHID      myHID(&Usb); //MyHID => HIDUniversial => USBHID 
MyReportParser                               Prs;

void setup()
{
    Serial.begin( 115200 );
#if !defined(__MIPSEL__)
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
    Serial.println("Start");

    if (Usb.Init() == -1)
        Serial.println("OSC did not start.");

    delay( 200 );

    myHID.SetReportParser(0, &Prs);

    Wire.begin(); // join i2c bus (address optional for master)
}

void loop()
{
  Usb.Task();
}
