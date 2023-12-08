#ifndef RemoteXYStream_USBSerial_h
#define RemoteXYStream_USBSerial_h


#if defined(USBCON)
#define RemoteXYStream_USBSerial_T Serial_
#elif defined(_WIRISH_USB_SERIAL_H_)
#define RemoteXYStream_USBSerial_T USBSerial
#elif defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
#define RemoteXYStream_USBSerial_T _SerialUSB
#endif

#if defined(RemoteXYStream_USBSerial_T)


#include "RemoteXYStream_Stream.h"


class CRemoteXYStream_USBSerial : public CRemoteXYStream_Stream {
  
  public:    
  CRemoteXYStream_USBSerial (RemoteXYStream_USBSerial_T * _serial, long _serialSpeed) : CRemoteXYStream_Stream () {
    _serial->begin (_serialSpeed);
    setStream (_serial);
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Init USB CDC serial ");
    RemoteXYDebugLog.writeAdd(_serialSpeed);
    RemoteXYDebugLog.writeAdd(" baud");
#endif
  }              
  
};


#endif  //RemoteXYStream_USBSerial_Type

#endif //RemoteXYStream_USBSerial_h