#ifndef _REMOTEXY_DEBUGLOG_H_
#define _REMOTEXY_DEBUGLOG_H_

#if defined(REMOTEXY__DEBUGLOG) 

#include <inttypes.h> 
#include <stdlib.h>
//#include <Arduino.h>
#include "RemoteXYFunc.h"

#ifndef REMOTEXY__DEBUGLOG_SERIAL 
#define REMOTEXY__DEBUGLOG_SERIAL Serial
#endif
#ifndef REMOTEXY__DEBUGLOG_SPEED 
#define REMOTEXY__DEBUGLOG_SPEED 115200
#endif

#ifndef REMOTEXY__DEBUGLOG_SERIAL_T 
#if defined(USBCON)
#define REMOTEXY__DEBUGLOG_SERIAL_T Serial_

// for Arduino Nano ESP32
#elif ARDUINO_ARCH_ESP32 && ARDUINO_USB_CDC_ON_BOOT && !ARDUINO_USB_MODE
#define REMOTEXY__DEBUGLOG_SERIAL_T USBCDC
#elif ARDUINO_ARCH_ESP32 && ARDUINO_USB_CDC_ON_BOOT && ARDUINO_USB_MODE
#define REMOTEXY__DEBUGLOG_SERIAL_T HWCDC

#else
#define REMOTEXY__DEBUGLOG_SERIAL_T HardwareSerial
#endif
#endif // REMOTEXY__DEBUGLOG_SERIAL_T


class CRemoteXYDebugLog {
  public:
  REMOTEXY__DEBUGLOG_SERIAL_T * serial;
  long speed;
  uint8_t inited;
  
  private:
  uint8_t debug_flags;
  uint8_t debug_hexcounter;
  
  public:
  CRemoteXYDebugLog (REMOTEXY__DEBUGLOG_SERIAL_T * _serial, long _speed) {
    debug_flags=0;
    serial = _serial;
    speed = _speed;
    inited = 0;
  }
  
  public:
  void init () {
    if (!inited) {
      serial->begin (speed);
      serial->println ();
      write (F("Debug log started"));
      inited = 1;
    }
  }


  public:
  void writeTime () {
    char sds[6];
    char sdm[4];
    uint32_t d = millis();
    rxy_intToFixedStr (d/1000, sds, 5, ' ');   
    rxy_intToFixedStr (d%1000, sdm, 3);  
    serial->println ();    
    serial->print (F("["));
    serial->print (sds);
    serial->print (F("."));
    serial->print (sdm);
    serial->print (F("] "));   
  }
  
  
  public:
  void write (const char *s) {
    debug_flags = 0;
    writeTime (); 
    serial->print(s);
  }
  
  public:
  void write (const __FlashStringHelper *s) {
    debug_flags = 0;
    writeTime (); 
    serial->print(s);
  }

  public:
  void writeAdd (const char *s) {
    serial->print(s);
  }

  public:
  void writeAdd (const __FlashStringHelper *s) {
    serial->print(s);
  }
  
  public:
  void writeAddChar (char c) {
    serial->write(c);
  }
  
  public:
  void writeAdd (uint16_t i) {
    serial->print(i);
  }

  public:
  void writeAdd (uint32_t i) {
    serial->print(i);
  }
      
  public:
  void writeAdd (int i) {
    serial->print(i);
  }
  
  public:
  void writeAdd (long i) {
    serial->print(i);
  }
    
  public:
  void writeInput () {
    if ((debug_flags & 0x01)==0) {
      writeTime ();
      serial->print(F("<- "));
      debug_hexcounter=0;
    }
    debug_flags = 0x01;   
  }
  
  public:
  void writeInput (const char *s) {
    writeInput ();  
    serial->print(s);
  }
  
  public:
  void writeInput (const __FlashStringHelper *s) {
    writeInput ();  
    serial->print(s);
  }

  public:
  void writeInputHex (uint8_t b) {
    writeInput ();  
    writeHex (b);
  }
  
  public:
  void writeInputChar (char s) {
    writeInput ();  
    serial->print(s);
  }  
  
  public:
  void writeOutput () {
    if ((debug_flags & 0x02)==0) {
      writeTime ();
      serial->print(F("-> "));
      debug_hexcounter=0;
    }
    debug_flags = 0x02;   
  }
  
  public:
  void writeOutput (const char *s) {
    writeOutput ();   
    serial->print(s);
  }
  
  public:
  void writeOutput (const __FlashStringHelper *s) {
    writeOutput ();   
    serial->print(s);
  }

  public:
  void writeOutputHex (uint8_t b) {
    writeOutput ();  
    writeHex (b);
  }
  

  public:
  void writeNewString () {
    debug_flags = 0;
  }
  
  public:
  void writeHex (uint8_t b) {
    debug_hexcounter++;
    if (debug_hexcounter>16) {
      serial->println();
      serial->print(F("              "));
      debug_hexcounter=1;
    }
    serial->print(' ');
    serial->print(b>>4, HEX); 
    serial->print(b&0x0f, HEX);     
  }   
  
  public:
  void writeAvailableMemory () {
    write (F("Available memory: "));
    writeAdd (availableMemory());
  }
  
  
  
  private:
  uint32_t availableMemory() {
#if defined (ESP8266) || defined (ESP32)
    return ESP.getFreeHeap ();
#elif defined (__AVR__)
    uint16_t size = RAMEND;
    uint8_t *buf;
    while ((buf = (uint8_t *)malloc(size)) == NULL)  size--;
    free(buf);
    return size;
#else
    return 0;
#endif
  }


};


CRemoteXYDebugLog RemoteXYDebugLog (& REMOTEXY__DEBUGLOG_SERIAL, REMOTEXY__DEBUGLOG_SPEED);


#endif  //REMOTEXY__DEBUGLOG

#endif //_REMOTEXY_DEBUGLOG_H_