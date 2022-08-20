#ifndef RemoteXYStream_AltSoftSerial_h
#define RemoteXYStream_AltSoftSerial_h

#if defined(AltSoftSerial_h)
// AltSoftSerial always uses these pins:
//
// Board          Transmit  Receive   PWM Unusable
// -----          --------  -------   ------------
// Teensy 3.0 & 3.1  21        20         22
// Teensy 2.0         9        10       (none)
// Teensy++ 2.0      25         4       26, 27
// Arduino Uno        9         8         10
// Arduino Leonardo   5        13       (none)
// Arduino Mega      46        48       44, 45
// Wiring-S           5         6          4
// Sanguino          13        14         12

#include "RemoteXYComm.h"


class CRemoteXYStream_AltSoftSerial : public CRemoteXYStream {
  
  private:
  AltSoftSerial * serial;
  
  public:
  CRemoteXYStream_AltSoftSerial (long _serialSpeed) : CRemoteXYStream () { 
    serial = new AltSoftSerial ();
    serial->begin (_serialSpeed);
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Init alt soft serial ");
    RemoteXYDebugLog.writeAdd(_serialSpeed);
    RemoteXYDebugLog.writeAdd(" baud");
#endif
  }              
  
  
  void handler () override {   
    while (serial->available ()) notifyReadByteListener (serial->read ());
  }
  
  void write (uint8_t byte) override {
    serial->write (byte);
  }

  
};


#endif  // AltSoftSerial_h

#endif //RemoteXYStream_AltSoftSerial_h