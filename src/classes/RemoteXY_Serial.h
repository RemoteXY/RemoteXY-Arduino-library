#ifndef _REMOTEXY_SERIAL_H_
#define _REMOTEXY_SERIAL_H_

#include "classes/RemoteXY_API.h"

class CRemoteXY_Serial : public CRemoteXY_API {

public:

#if defined(REMOTEXY_PORT__HARDSERIAL)
  HardwareSerial * serial;
  void initSerial (HardwareSerial * _serial, long _serialSpeed) {
    serial = _serial;
    #if defined(REMOTEXY_MODE__BLE_NANO)
      Serial.begin (_serialSpeed);
    #else
      serial->begin (_serialSpeed);
    #endif
  }
#elif defined(REMOTEXY_PORT__SOFTSERIAL)
  #if defined(SoftwareSerial_h)
    SoftwareSerial * serial;
    void initSerial (uint8_t _serialRx, uint8_t _serialTx, long _serialSpeed) {
      serial = new SoftwareSerial (_serialRx, _serialTx);
      serial->begin (_serialSpeed);
    }
  #elif defined(SoftSerial_h)
    SoftSerial * serial;
    void initSerial (uint8_t _serialRx, uint8_t _serialTx, long _serialSpeed) {
      serial = new SoftSerial (_serialRx, _serialTx);
      serial->begin (_serialSpeed);
    }
  #endif
#endif
};

#endif //_REMOTEXY_SERIAL_H_
