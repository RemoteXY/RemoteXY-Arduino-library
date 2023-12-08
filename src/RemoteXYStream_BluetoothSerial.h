#ifndef RemoteXYStream_BluetoothSerial_h
#define RemoteXYStream_BluetoothSerial_h

#if defined (BluetoothSerial_h) || defined (_BLUETOOTH_SERIAL_H_)

#include "RemoteXYStream_Stream.h"


class CRemoteXYStream_BluetoothSerial : public CRemoteXYStream_Stream {
  
  public:
  CRemoteXYStream_BluetoothSerial (const char * _bleDeviceName) : CRemoteXYStream_Stream () {
    BluetoothSerial *serial = new BluetoothSerial ();
    serial->begin (_bleDeviceName);
    setStream (serial);
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Init bluetooth serial: ");
    RemoteXYDebugLog.writeAdd(_bleDeviceName);
#endif
  }              

};

#endif // BluetoothSerial_h   _BLUETOOTH_SERIAL_H_
#endif //RemoteXYStream_BluetoothSerial_h