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

#include "RemoteXYStream_Stream.h"


class CRemoteXYStream_AltSoftSerial : public CRemoteXYStream_Stream {
  
  
  public:
  CRemoteXYStream_AltSoftSerial (long _serialSpeed) : CRemoteXYStream_Stream () { 
    AltSoftSerial serial = new AltSoftSerial ();
    serial->begin (_serialSpeed);
    setStream (serial);
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Init alt soft serial ");
    RemoteXYDebugLog.writeAdd(_serialSpeed);
    RemoteXYDebugLog.writeAdd(" baud");
#endif
  }              
  
};


#endif  // AltSoftSerial_h

#endif //RemoteXYStream_AltSoftSerial_h