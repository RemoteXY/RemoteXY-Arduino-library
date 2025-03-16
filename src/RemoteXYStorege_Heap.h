#ifndef RemoteXYStorage_Heap_h
#define RemoteXYStorage_Heap_h

#include "RemoteXYFunc.h"
#include "RemoteXYType.h"

#pragma pack(push, 1)
struct RemoteXYStorage_Heap_Head {
  uint16_t len;  
  uint16_t id;
};
#pragma pack(pop)

class RemoteXYStorage_Heap {
  
  private:
  uint8_t * heap;
  uint16_t heapSize;
  uint16_t firstIndex;
  uint16_t fillSize; // number of bytes used in the heap    
  uint16_t nextId;
  uint16_t index;
  
  
  public:
  RemoteXYStorage_Heap () {
    firstIndex = 0;
    fillSize = 0;
    nextId = 1;  
  }
  
  protected:
  void initHeap (uint8_t * _heap, uint16_t _heapSize) {
    heap = _heap;
    heapSize = _heapSize;
  } 
  
  
  private:
  void addIndex (uint16_t d) {
    index += d;
    if (index >= heapSize) index -= heapSize;
  }
  
  private:
  void incIndex () {
    index ++;
    if (index >= heapSize) index = 0;
  }
  
  public:
  uint8_t getNextByte () {
    uint8_t b = heap[index];
    incIndex ();
    return b;  
  }
  
  void putNextByte (uint8_t b) {
    heap[index] =  b;
    incIndex ();
  }  
  
  private:
  void fillHead (RemoteXYStorage_Heap_Head * head) {
    uint8_t * h = (uint8_t*)head;
    for (uint8_t i = 0; i < sizeof (RemoteXYStorage_Heap_Head); i++) {
      *h++ = getNextByte ();
    }
  }
  
  private:
  void saveHead (RemoteXYStorage_Heap_Head * head) {
    uint8_t * h = (uint8_t*)head;
    for (uint8_t i = 0; i < sizeof (RemoteXYStorage_Heap_Head); i++) {
      putNextByte (*h++);
    }
  }
  
  // if the heap is full, the first message will be deleted
  public:
  uint8_t allocateNew (uint16_t len) {
    uint16_t size = len + sizeof (RemoteXYStorage_Heap_Head);  
    if (size > heapSize) {
      // new message is so long
      return 0;
    }
    
    while (size > heapSize-fillSize) {
      removeFirst ();
    }
    RemoteXYStorage_Heap_Head head;
    head.len = len;
    head.id = nextId++;
    
    index = firstIndex;
    addIndex (fillSize);
    saveHead (&head);

    fillSize+=size; 
    return 1;
  }

  
  public:
  void removeFirst () {
    RemoteXYStorage_Heap_Head head;
    index = firstIndex;
    fillHead (&head);
    addIndex (head.len);
    firstIndex = index;
    fillSize -= head.len + sizeof (RemoteXYStorage_Heap_Head); 
  }  
  
  public:
  uint8_t notEmpty () {
    if (fillSize > 0) return 1;
    return 0;
  }
  
 
  public:
  RemoteXYStorage_Heap_Head getFirstHead () {
    RemoteXYStorage_Heap_Head head;
    index = firstIndex;
    fillHead (&head);
    return head;
  }
      
  // add data to end of heap
  public:
  uint8_t add (uint8_t * data, uint16_t len) {
    if (allocateNew (len)) {
      while (len--) putNextByte (*data++);
      return 1;
    }
    return 0;
  }  
  
    
};

class RemoteXYStorage_Heap_64 : public RemoteXYStorage_Heap {
  public:
  uint8_t heapBytes[64];  
  RemoteXYStorage_Heap_64 () : RemoteXYStorage_Heap() {
    initHeap (heapBytes, 64);
  }
};

class RemoteXYStorage_Heap_128 : public RemoteXYStorage_Heap {
  public:
  uint8_t heapBytes[128];  
  RemoteXYStorage_Heap_128 () : RemoteXYStorage_Heap() {
    initHeap (heapBytes, 128);
  }
};

class RemoteXYStorage_Heap_256 : public RemoteXYStorage_Heap {
  public:
  uint8_t heapBytes[256];  
  RemoteXYStorage_Heap_256 () : RemoteXYStorage_Heap() {
    initHeap (heapBytes, 256);
  }
};

class RemoteXYStorage_Heap_512 : public RemoteXYStorage_Heap {
  public:
  uint8_t heapBytes[512];  
  RemoteXYStorage_Heap_512 () : RemoteXYStorage_Heap() {
    initHeap (heapBytes, 512);
  }
};

class RemoteXYStorage_Heap_1024 : public RemoteXYStorage_Heap {
  public:
  uint8_t heapBytes[1024];  
  RemoteXYStorage_Heap_1024 () : RemoteXYStorage_Heap() {
    initHeap (heapBytes, 1024);
  }
};

class RemoteXYStorage_Heap_2048 : public RemoteXYStorage_Heap {
  public:
  uint8_t heapBytes[2048];  
  RemoteXYStorage_Heap_2048 () : RemoteXYStorage_Heap() {
    initHeap (heapBytes, 2048);
  }
};

#endif // RemoteXYStorage_Heap_h