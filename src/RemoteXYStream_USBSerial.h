#ifndef RemoteXYStream_USBSerial_h
#define RemoteXYStream_USBSerial_h


#if defined(USBCON)
#define RemoteXYStream_USBSerial_Type Serial_

#elif defined(_WIRISH_USB_SERIAL_H_)
#define RemoteXYStream_USBSerial_Type USBSerial

#endif

#if defined(RemoteXYStream_USBSerial_Type)


#include "RemoteXYStream.h"


class CRemoteXYStream_USBSerial : public CRemoteXYStream {
  
  private:
  RemoteXYStream_USBSerial_Type * serial;
  
  public:
  CRemoteXYStream_USBSerial (RemoteXYStream_USBSerial_Type * _serial, long _serialSpeed) : CRemoteXYStream () {
    serial = _serial;
    serial->begin (_serialSpeed);
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Init USB CDC serial ");
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


#endif  //RemoteXYStream_USBSerial_Type

#endif //RemoteXYStream_USBSerial_h