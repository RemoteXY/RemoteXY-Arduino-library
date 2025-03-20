#ifndef RemoteXYData_h
#define RemoteXYData_h


#include "RemoteXYEeprom.h"  
#include "RemoteXYNet.h"

#include "RemoteXYRealTime.h"

class CRemoteXYNet;

class CRemoteXYData {

  public:
  
  CRemoteXYNet * nets;  
  CRemoteXYRealTime * realTime;


#if defined(REMOTEXY_HAS_EEPROM)
  public:
  CRemoteXYEeprom eeprom;
  CRemoteXYEepromItem * boardId; 
#endif 
           
                   
};

#endif //RemoteXYData_h