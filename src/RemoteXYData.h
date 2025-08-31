#ifndef RemoteXYData_h
#define RemoteXYData_h


#include "RemoteXYEeprom.h"  
#include "RemoteXYNet.h"



class CRemoteXYNet;

class CRemoteXYData {

  public:
  
  CRemoteXYNet * nets;  
  
  int64_t boardTime;   
  uint32_t handlerMillis;



#if defined(REMOTEXY_HAS_EEPROM)
  public:
  CRemoteXYEeprom eeprom;
    
#endif 
           
                   
};

#endif //RemoteXYData_h