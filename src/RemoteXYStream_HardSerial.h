#ifndef RemoteXYStream_HardSerial_h
#define RemoteXYStream_HardSerial_h

#include "RemoteXYStream_Stream.h"


class CRemoteXYStream_HardSerial : public CRemoteXYStream_Stream {
    
  public:
  CRemoteXYStream_HardSerial (HardwareSerial * _serial, long _serialSpeed) : CRemoteXYStream_Stream () {
    _serial->begin (_serialSpeed);
    setStream (_serial);
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Init hardware serial ");
    RemoteXYDebugLog.writeAdd(_serialSpeed);
    RemoteXYDebugLog.writeAdd(" baud");
#endif
  }              
  
};


#endif //RemoteXYStream_HardSerial_h