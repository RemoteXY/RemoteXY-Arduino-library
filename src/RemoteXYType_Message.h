#ifndef RemoteXYType_Message_h
#define RemoteXYType_Message_h

#include "RemoteXYFunc.h"
#include "RemoteXYTypeUseHeap.h"


class RemoteXYType_Message : public CRemoteXYTypeUseHeap {
    
  public:
  uint16_t sizeOf () override {
    return sizeof (RemoteXYType_Message);
  }
         
  public:
  uint8_t send (const char * message) {
    uint16_t len = rxy_strLength (message); 
    return addToHeap ((uint8_t*)message, len);
  }
  
  public:
  uint8_t send (const char * message, uint16_t len) {
    return addToHeap ((uint8_t*)message, len);
  }
   
};


#endif // RemoteXYType_Message_h