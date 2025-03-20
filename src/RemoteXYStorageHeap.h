#ifndef RemoteXYStorageHeap_h
#define RemoteXYStorageHeap_h

#include "RemoteXYFunc.h"
#include "RemoteXYType.h"

#pragma pack(push, 1)
struct RemoteXYStorageHeap_Head {
  uint16_t id;
  uint16_t len;  
};
#pragma pack(pop)

class CRemoteXYStorageHeap {
  
  private:
  uint8_t * heap;
  uint16_t heapSize;
  uint16_t firstIndex;
  uint16_t fillSize; // number of bytes used in the heap    
  uint16_t index;
  
  public:
  uint16_t lastId;
  uint16_t count;
  
  
  public:
  CRemoteXYStorageHeap () {
    firstIndex = 0;
    fillSize = 0;
    lastId = 0;  
    heapSize = 0;
    count = 0;
  }
  
  public:
  uint8_t init (uint16_t _heapSize) {
    heap = (uint8_t*)malloc (_heapSize);
    if (heap == NULL) {
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write("Out of RAM for StorageHeap: ");              
      RemoteXYDebugLog.writeAdd(_heapSize);              
#endif    
      return 0; 
    }
    heapSize = _heapSize;
    return 1;
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
  void fillHead (RemoteXYStorageHeap_Head * head) {
    uint8_t * h = (uint8_t*)head;
    for (uint8_t i = 0; i < sizeof (RemoteXYStorageHeap_Head); i++) {
      *h++ = getNextByte ();
    }
  }
  
  private:
  void saveHead (RemoteXYStorageHeap_Head * head) {
    uint8_t * h = (uint8_t*)head;
    for (uint8_t i = 0; i < sizeof (RemoteXYStorageHeap_Head); i++) {
      putNextByte (*h++);
    }
  }
  
  // if the heap is full, the first message will be deleted
  public:
  uint8_t allocateNew (uint16_t len) {
    uint16_t size = len + sizeof (RemoteXYStorageHeap_Head);  
    if (size > heapSize) {
      // new message is so long
      return 0;
    }
    
    while (size > heapSize-fillSize) {
      removeFirst ();
    }
    RemoteXYStorageHeap_Head head;
    head.len = len;
    lastId++;
    head.id = lastId;
    
    index = firstIndex;
    addIndex (fillSize);
    saveHead (&head);

    fillSize+=size; 
    count++;
    return 1;
  }

  
  public:
  void removeFirst () {
    RemoteXYStorageHeap_Head head;
    index = firstIndex;
    fillHead (&head);
    addIndex (head.len);
    firstIndex = index;
    fillSize -= head.len + sizeof (RemoteXYStorageHeap_Head); 
    count--;
  }  
  
  public:
  uint8_t empty () {
    return count == 0;
  }
  
 
  public:
  uint8_t getFirst (RemoteXYStorageHeap_Head * head) {
    if (count == 0) return 0;
    index = firstIndex;
    fillHead (head);
    return 1;
  }
  
  public:
  uint8_t findById (uint16_t id, RemoteXYStorageHeap_Head * head) {
    if (count == 0) return 0;
    uint16_t cnt = count;
    index = firstIndex;    
    while (cnt--) {
      fillHead (head);
      if (head->id == id) return 1;
      addIndex (head->len);
    }
    return 0;
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
  
  public:
  uint8_t add (uint8_t * data1, uint16_t len1, uint8_t * data2, uint16_t len2) {
    if (allocateNew (len1 + len2)) {
      while (len1--) putNextByte (*data1++);
      while (len2--) putNextByte (*data2++);
      return 1;
    }
    return 0;
  }         
      
};


#endif // RemoteXYStorageHeap_h