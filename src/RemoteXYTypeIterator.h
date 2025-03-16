#ifndef RemoteXYTypeIterator_h
#define RemoteXYTypeIterator_h


#include "RemoteXYGuiData.h"
#include "RemoteXYType_Message.h"
#include "RemoteXYType_Notification.h"



class RemoteXYTypeIterator {
  public:
  CRemoteXYGuiData * data;
  uint8_t *conf;  
  uint16_t count;
  uint8_t spec;
  RemoteXYType * var;

  
  void start (CRemoteXYGuiData * _data) {
    data = _data;
    conf = data->complexVarConf;
    count = data->complexVarCount;
    var = NULL; 
  }

  
  uint8_t next () {
    if (var == NULL) {
      var = (RemoteXYType*)data->complexVar;
    }
    else {
      var = (RemoteXYType*)(((uint8_t*)var) + getSize ());
    }    
    if (count) {
      spec = rxy_readConfByte (conf++);     
      count--;
      return 1;
    }  
    return 0;
  }   
  
  // spec may be defined
  uint16_t getSize () {
    uint8_t tp = spec >> 4;
    uint8_t dat = spec & 0x0f;
    if (tp == 1) {
      if (dat == 0) {}
    }
#if defined REMOTEXY_HAS_EEPROM
    if (tp == 2) {
      if (dat == 0) return sizeof (RemoteXYType_Notification_64);
      if (dat == 1) return sizeof (RemoteXYType_Notification_128);
      if (dat == 2) return sizeof (RemoteXYType_Notification_256);
      if (dat == 3) return sizeof (RemoteXYType_Notification_512);
      return 0;
    }
#endif
    return 0;
  }
  
  
};


#endif // RemoteXYTypeIterator_h