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
  RemoteXYEeprom eeprom;
  RemoteXYEepromItem * boardId; 
#endif 
           
  RemoteXYTimeStamp getBoardTime () {
    if (realTime != NULL) return realTime->getBoardTime ();
    return RemoteXYTimeStamp();
  }         
                   
};

#endif //RemoteXYData_h