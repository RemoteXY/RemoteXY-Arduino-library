#ifndef RemoteXYData_h
#define RemoteXYData_h


#include "RemoteXYEeprom.h"  
#include "RemoteXYNet.h"



class CRemoteXYGui;

class CRemoteXYData {

  public:
  
  CRemoteXYNet * nets;  
  CRemoteXYGui * guis;
  
  int64_t boardTime;   
  uint32_t handlerMillis;


#if defined(REMOTEXY_HAS_EEPROM)
  public:
  CRemoteXYEeprom eeprom;
    
#endif 
           
  public:  
  void addNet (CRemoteXYNet * net) {  
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.init ();
    RemoteXYDebugLog.write(F("Add net"));
#endif 
    if (net) {
      CRemoteXYNet * p = nets;
      while (p) {
        if (p == net) return;
        p = p->next;
      }
      net->next = nets;
      nets = net;
    }
#if defined(REMOTEXY__DEBUGLOG) 
    else {
      RemoteXYDebugLog.write(F("Out of memory for addNet()"));
    }
#endif    
  } 
  
                     
};

#endif //RemoteXYData_h