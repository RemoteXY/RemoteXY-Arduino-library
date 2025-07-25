Library version: 4.1.2

This code is the library for Arduino IDE. The library is part of the RemoteXY project. 
RemoteXY allows you to control your device using an Android or iOS smartphone. There are Bluetooth, WiFi, Ethernet, USB OTG or Cloud server can be used for connection.

For more details and make GUI go to website http://remotexy.com/


History:
   version 2.3.1    
     - Support the device access password;
     - Support the cloud server as beta test;
     - Fixed a bug where the length of variables more than 255;
     - Fixed a bug where ESP module reboot and device did not see it;
     - Fixed a bug where the connection was filed and the device 
         did not see it and reconnection is impossible 
   version 2.3.4  
     - Fixed a bug where the length of all input variables more than 256;
     - Fixed a bug where millis() overflow in 50 days;
     - Fixed some bugs;
   version 2.3.5  
     - Fixed some bugs;
   version 2.4.1
     - support ESP32 WiFi and Bluetooth   
   version 2.4.2
     - Fixed some bugs;
   version 2.4.3
     - Fixed some bugs;
   version 2.4.4
     - Fixed ESP32 BLE bugs;
   version 3.1.1
     - Full update the library: multiple connections, set input variables from board and more;
   version 3.1.3
     - Multiple connection for cloud, fixed some bugs;
   version 3.1.4
     - fixed some bugs;
   version 3.1.5
     - add NRF5XXX and BLEPeripheral.h library as Beta, need define REMOTEXY_MODE__NRFCORE_BLEPERIPHERAL;
   version 3.1.6
     - fixed some bugs;
   version 3.1.7
     - add USB CDC Serial for Leonardo, Micro, ..;     
   version 3.1.8
     - fixed some bugs;
   version 3.1.9
     - added function RemoteXY_delay (ms) as alternative delay (ms);
   version 3.1.10
     - added suppot for AltSoftSerial.h library;
     - support SoftwareSerial.h for ESP8266;
   version 3.1.11
     - add USB CDC serial for STM32;
   version 3.1.12
     - fixed some bugs;
     - any Stream class can be used for communication: example/RemoteXY/Various/AnyStreamClass.pde
   version 3.1.13
     - fixed some bugs;
     - added support for the arduinoBLE.h library
     - added support for the WiFiS3.h library, which uses Arduino UNO R4 WiFi
   version 3.1.14
     - support ESP32 ver.3.0.x     
     
   version 4.1.1 (beta)
     - library update to support new features
   version 4.1.2
     - MIT license
     - support Arduino NANO 33 BLE     