# Summary
A USB HID Duplicator is a device which duplicates report of USB Human Interface Device(HID), such as mouse, keyboard, joystick, etc.  
So that, one HID device can be used to control multiple computers.

# Demo Video
|Mouse    |Keyboard | Wireless Presenter     |
|---------|---------|------------------------|
|[![USB HID Duplicator Demo(Mouse)](http://img.youtube.com/vi/KWI-PguVAN8/0.jpg)](http://www.youtube.com/watch?v=KWI-PguVAN8 "USB HID Duplicator Demo(Mouse)")     |[![USB HID Duplicator demo (Keyboard)](http://img.youtube.com/vi/Ez5Jvu0lLoQ/0.jpg)](http://www.youtube.com/watch?v=Ez5Jvu0lLoQ "USB HID Duplicator demo (Keyboard)")   |[![USB HID Duplicator demo (Wireless Presenter)](http://img.youtube.com/vi/Hjw7fimRGSo/0.jpg)](http://www.youtube.com/watch?v=Hjw7fimRGSo "USB HID Duplicator demo (Wireless Presenter)")   |
# Component
A USB HID Duplicator consists of a master([USB Host Shield](https://github.com/felis/USB_Host_Shield_2.0) + Arduino UNO) and slaves(Arduino Leonardo)

A master and slaves are connected via I2C serial communication.

# How To Use?
1. Put "MyHID" folder into your Arduino user library directory.
> * Arduino/
>    * libraries/
>        * MyHID
2. Upload "master" sketch to master(Arduino UNO board with USB Host Shield)
3. Upload "slave" sketch to slaves(Arduino Leonardo board)
4. Connect matster and slaves with I2C  
     You need to connect SCL(A5 for UNO), SDA(A4 for Arduino UNO), GND pin
 5. Connect slaves to respective computers.(one slave for one computer)
 6. Power up mater.
 7. Done! Connect a USB HID device(ex. keyboard) to master and use it.

# Reference
* USB Host Shield
  * https://github.com/felis/USB_Host_Shield_2.0/edit/master/README.md
  * https://chome.nerpa.tech/arduino_usb_host_shield_projects/
* USBHID library  
  * https://github.com/arduino/ArduinoCore-avr/tree/master/libraries/HID/src
* USB made simple
  * This is a greate website to learn about USB!
  * http://www.usbmadesimple.co.uk/index.html
* Tutorial about USB HID Report Descriptors
  * https://eleccelerator.com/tutorial-about-usb-hid-report-descriptors/

