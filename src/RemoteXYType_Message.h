#ifndef RemoteXYType_Message_h
#define RemoteXYType_Message_h

#include "RemoteXYFunc.h"
#include "RemoteXYType.h"
#include "RemoteXYStorege_Heap.h"

// messages are stored in the heap



class RemoteXYType_Message : public RemoteXYType {
  
  protected:
  RemoteXYStorage_Heap * heap;
  
  public:
  RemoteXYType_Message () {
  }
    
  
  public:
  uint8_t send (const char * message) {
    uint16_t len = rxy_strLength (message); 
    return heap->add ((uint8_t*)message, len);
  }
  
};


class RemoteXYType_Message_64 : public RemoteXYType_Message {
  public:
  RemoteXYStorage_Heap_64 heapInstance;    
  RemoteXYType_Message_64 () : RemoteXYType_Message() {
    heap = &heapInstance;
  }
};

class RemoteXYType_Message_128 : public RemoteXYType_Message {
  public:
  RemoteXYStorage_Heap_128 heapInstance; 
  RemoteXYType_Message_128 () : RemoteXYType_Message() {
    heap = &heapInstance;
  }
};



#endif // RemoteXYType_Message_h