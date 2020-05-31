#ifndef _REMOTEXY_MOD__ESP32CORE_BLUETOOTH_H_
#define _REMOTEXY_MOD__ESP32CORE_BLUETOOTH_H_

#include "BluetoothSerial.h"
#include "classes/RemoteXY_Serial.h"
     

class CRemoteXY : public CRemoteXY_API {

public:
  BluetoothSerial  * serial;
  CRemoteXY (const void * _conf, void * _var, const char * _accessPassword, const char * _bleDeviceName) {
    serial = new  BluetoothSerial ();
    serial->begin (_bleDeviceName);
    init (_conf, _var, _accessPassword); 
  }


  protected:

  void sendByte (uint8_t b) {
    serial->write (b);
#if defined(REMOTEXY__DEBUGLOGS)
    DEBUGLOGS_writeOutputHex (b);
#endif
  }
  
  uint8_t receiveByte () {
    uint8_t b = serial->read ();
#if defined(REMOTEXY__DEBUGLOGS)
    DEBUGLOGS_writeInputHex (b); 
#endif
    return b;
  }
  
  
  uint8_t availableByte () {
    return serial->available ();
  };  


};


#define RemoteXY_Init() remotexy = new CRemoteXY (RemoteXY_CONF_PROGMEM, &RemoteXY, REMOTEXY_ACCESS_PASSWORD, REMOTEXY_BLUETOOTH_NAME)

#endif //_REMOTEXY_MOD__ESP32CORE_BLUETOOTH_H_