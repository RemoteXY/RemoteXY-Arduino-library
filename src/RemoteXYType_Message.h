#ifndef RemoteXYType_Message_h
#define RemoteXYType_Message_h

#include "RemoteXYFunc.h"
#include "RemoteXYType.h"
#include "RemoteXYStorege_Heap.h"

// messages are stored in the heap



class CRemoteXYType_Message : public CRemoteXYType {
  
  protected:
  CRemoteXYStorage_Heap * heap;
  
  public:
  CRemoteXYType_Message () {
  }
    
  
  public:
  uint8_t send (const char * message) {
    uint16_t len = rxy_strLength (message); 
    return heap->add ((uint8_t*)message, len);
  }
  
};


class RemoteXYType_Message_64 : public CRemoteXYType_Message {
  public:
  CRemoteXYStorage_Heap_64 heapInstance;    
  RemoteXYType_Message_64 () : CRemoteXYType_Message() {
    heap = &heapInstance;
  }
};

class RemoteXYType_Message_128 : public CRemoteXYType_Message {
  public:
  CRemoteXYStorage_Heap_128 heapInstance; 
  RemoteXYType_Message_128 () : CRemoteXYType_Message() {
    heap = &heapInstance;
  }
};



#endif // RemoteXYType_Message_h