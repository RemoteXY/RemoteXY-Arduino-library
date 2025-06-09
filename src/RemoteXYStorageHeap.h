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
  uint16_t index;  // for iterators
  
  public:
  uint16_t firstId;
  uint16_t lastId;
  uint16_t count;
  
  
  public:
  CRemoteXYStorageHeap () {
    firstIndex = 0;
    fillSize = 0;
    firstId = 0;
    lastId = 0xffff;  
    heapSize = 0;
    count = 0;
  }
  
  public:
  uint8_t init (uint16_t _heapSize) {
    heap = (uint8_t*)malloc (_heapSize);
    if (heap == NULL) {
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write(F("Out of RAM for StorageHeap: "));              
      RemoteXYDebugLog.writeAdd(_heapSize);              
#endif    
      return 0; 
    }
    heapSize = _heapSize;
    return 1;
  } 
  
  
  private:
  void addIndex (uint16_t d) {
    if (heapSize - d <= index)  index -= (heapSize - d); 
    else index += d;
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
    uint8_t * p = (uint8_t*)head;
    for (uint8_t i = 0; i < sizeof (RemoteXYStorageHeap_Head); i++) {
      *p++ = getNextByte ();
    }
  }
  
  private:
  void saveHead (RemoteXYStorageHeap_Head * head) {
    uint8_t * p = (uint8_t*)head;
    for (uint8_t i = 0; i < sizeof (RemoteXYStorageHeap_Head); i++) {
      putNextByte (*p++);
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
    lastId++;
    RemoteXYStorageHeap_Head head;
    head.len = len;
    head.id = lastId;
    if (count == 0) firstId = lastId;
    
    index = firstIndex;
    addIndex (fillSize);
    saveHead (&head);

    fillSize += size; 
    count++;
    return 1;
  }

  
  public:
  void removeFirst () {
    RemoteXYStorageHeap_Head head;
    index = firstIndex;
    fillHead (&head);
    fillSize -= head.len + sizeof (RemoteXYStorageHeap_Head);     
    addIndex (head.len);
    firstIndex = index;
    count--;
    if (count > 0) {
      fillHead (&head);
      firstId = head.id;    
    }
  }  
  
  public:
  uint8_t empty () {
    return count == 0;
  }

  
  public:
  uint8_t containsId (uint16_t id) {
    if (count == 0) return 0;
    if (firstId > lastId) return (id >= firstId) || (id <= lastId);
    else return (id >= firstId) && (id <= lastId);
  }
  
  public:
  uint8_t findById (uint16_t id, RemoteXYStorageHeap_Head * head) {
    if (containsId (id)) {
      uint16_t cnt = count;
      index = firstIndex;  
      while (cnt--) {
        fillHead (head);
        if (head->id == id) return 1;
        addIndex (head->len);
      }
    }
    return 0;
  }
  
  // return length
  uint16_t takeBytes (uint16_t id) {
    if (containsId (id)) {
      uint16_t len = fillSize;
      uint16_t cnt = count;
      uint16_t bytesIndex;
      RemoteXYStorageHeap_Head head;
      index = firstIndex;  
      while (cnt--) {
        bytesIndex = index;
        fillHead (&head);
        if (head.id == id) {
          index = bytesIndex;
          return len;
        }
        len -= (head.len + sizeof (RemoteXYStorageHeap_Head));
        addIndex (head.len);
      }      
    }
    return 0;
  }
  
  
      
  // add data to end of heap
  public:
  uint8_t add (const uint8_t * data, uint16_t len) {
    if (allocateNew (len)) {
      while (len--) putNextByte (*data++);
      return 1;
    }
    return 0;
  }  
  
  public:
  uint8_t add (const uint8_t * data1, uint16_t len1, const uint8_t * data2, uint16_t len2) {
    if (allocateNew (len1 + len2)) {
      while (len1--) putNextByte (*data1++);
      while (len2--) putNextByte (*data2++);
      return 1;
    }
    return 0;
  }         
      
};



#endif // RemoteXYStorageHeap_h