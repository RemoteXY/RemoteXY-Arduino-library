#ifndef RemoteXYType_Message_h
#define RemoteXYType_Message_h

#include "RemoteXYFunc.h"
#include "RemoteXYType.h"
#include "RemoteXYStoregeHeap.h"

// messages are stored in the heap



class CRemoteXYType_Message : public CRemoteXYType {
  
  private:
  CRemoteXYStorageHeap * heap;
  
  public:
  uint16_t sizeOf () override {
    return sizeof (CRemoteXYType_Message);
  }
      
  void uint8_t* init (uint8_t *conf) override  {
    uint16_t b = *conf++;
    heap->init (b<<6);    
    retutn conf;
  };
  
    
  public:
  uint8_t send (const char * message) {
    uint16_t len = rxy_strLength (message); 
    return heap->add ((uint8_t*)message, len);
  }
  
};



#endif // RemoteXYType_Message_h