#ifndef RemoteXYStream_SoftSerial_h
#define RemoteXYStream_SoftSerial_h


// if not defined SoftwareSerial.h
#if defined(REMOTEXY_MODE__SOFTSERIAL) || defined(REMOTEXY_MODE__ESP8266_SOFTSERIAL_POINT) || defined(REMOTEXY_MODE__SOFTSERIAL_ESP8266) || defined(REMOTEXY_MODE__SOFTSERIAL_ESP8266_CLOUD)
  #if !defined (SoftwareSerial_h) && !defined(SoftSerial_h)
    #include "SoftwareSerial.h"
  #endif  
#endif


#if defined(SoftwareSerial_h) || defined(__SoftwareSerial_h) || defined(__SOFTWARE_SERIAL_H__)
#define RemoteXYStream_SoftSerial_T SoftwareSerial
#elif defined(SoftSerial_h)
#define RemoteXYStream_SoftSerial_T SoftSerial
#endif



#if defined(RemoteXYStream_SoftSerial_T)

#include "RemoteXYStream_Stream.h"


class CRemoteXYStream_SoftSerial : public CRemoteXYStream_Stream {
  
  public:
  CRemoteXYStream_SoftSerial (uint8_t _serialRx, uint8_t _serialTx, long _serialSpeed) : CRemoteXYStream_Stream () { 
    RemoteXYStream_SoftSerial_T * serial = new RemoteXYStream_SoftSerial_T (_serialRx, _serialTx);
    serial->begin (_serialSpeed);
    setStream (serial);
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Init software serial ");
    RemoteXYDebugLog.writeAdd(_serialSpeed);
    RemoteXYDebugLog.writeAdd(" baud");
    RemoteXYDebugLog.write("pin RX=");
    RemoteXYDebugLog.writeAdd(_serialRx);
    RemoteXYDebugLog.writeAdd("; pin TX=");
    RemoteXYDebugLog.writeAdd(_serialTx);
#endif
  }              
  
};


#endif  // RemoteXYStream_SoftSerial_T

#endif //RemoteXYStream_SoftSerial_h