#ifndef RemoteXYStream_SoftSerial_h
#define RemoteXYStream_SoftSerial_h

#if defined(SoftwareSerial_h) || defined(SoftSerial_h)

#include "RemoteXYComm.h"


class CRemoteXYStream_SoftSerial : public CRemoteXYStream {
  
#if defined(SoftwareSerial_h)

  private:
  SoftwareSerial * serial;
  
  public:
  CRemoteXYStream_SoftSerial (uint8_t _serialRx, uint8_t _serialTx, long _serialSpeed) : CRemoteXYStream () { 
    serial = new SoftwareSerial (_serialRx, _serialTx);
    serial->begin (_serialSpeed);
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
  
#elif defined(SoftSerial_h)

  private:
  SoftSerial * serial;
  
  public:
  CRemoteXYStream_SoftSerial (uint8_t _serialRx, uint8_t _serialTx, long _serialSpeed) : CRemoteXYClient () { 
    serial = new SoftSerial (_serialRx, _serialTx);
    serial->begin (_serialSpeed);
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Init soft serial ");
    RemoteXYDebugLog.writeAdd(_serialSpeed);
    RemoteXYDebugLog.writeAdd(" baud");
    RemoteXYDebugLog.write("pin RX=");
    RemoteXYDebugLog.writeAdd(_serialRx);
    RemoteXYDebugLog.writeAdd("; pin TX=");
    RemoteXYDebugLog.writeAdd(_serialTx);
#endif
  }              


#endif  
  
  void handler () override {   
    while (serial->available ()) notifyReadByteListener (serial->read ());
  }
  
  void write (uint8_t byte) override {
    serial->write (byte);
  }

  
};


#endif  // SoftwareSerial_h  SoftSerial_h

#endif //RemoteXYStream_SoftSerial_h