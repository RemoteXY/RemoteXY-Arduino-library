#ifndef RemoteXYData_h
#define RemoteXYData_h


#include "RemoteXYEeprom.h"  
#include "RemoteXYNet.h"
#include "RemoteXYRealTime.h"

class CRemoteXYNet;

class CRemoteXYData {

  public:
  
  CRemoteXYNet * nets;    
  //CRemoteXYHttpRequest_Proto * httpRequest;  
  CRemoteXYRealTime * realTime;


#if defined(REMOTEXY_HAS_EEPROM)
  public:
  RemoteXYEeprom eeprom;
  RemoteXYEepromItem * boardId; 
#endif 
             
             /*         
  public:
  CRemoteXYHttpRequest_Proto * getHttpRequest () {
    if (httpRequest == NULL) {
      CRemoteXYNet * net = nets; 
      while (net) {
        httpRequest = ((CRemoteXYNet*)net)->createHttpRequest (); 
        if (httpRequest != NULL) break;
        net = net->next;
      } 
    }
    return httpRequest;
  }            
  */          
};

#endif //RemoteXYData_h