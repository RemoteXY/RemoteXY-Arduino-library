#ifndef RemoteXYType_Heap_h
#define RemoteXYType_Heap_h

#include "RemoteXYStorageHeap.h"
#include "RemoteXYType.h"
#include "RemoteXYThread.h"


#pragma pack(push, 1)
struct CRemoteXYTypeInner_Heap_Descriptor {
  uint16_t count;
  uint16_t lastId;
};

struct CRemoteXYTypeInner_Heap_PackageHead {
  uint16_t num;
  uint16_t id;
};
#pragma pack(pop)

// VIRTUAL
class CRemoteXYTypeInner_Heap : public CRemoteXYTypeInner {
  protected:
  CRemoteXYStorageHeap heap;
  
  public:
  uint8_t* init (uint8_t *conf) override  {
    uint16_t len;
    len = rxy_readConfByte (conf++);
    len |= rxy_readConfByte (conf++)<<8;  
    if (len < 2) len = 2; // min 32 bytes
    heap.init (len << 4);    
    return conf;
  };
  
  public:
  uint8_t addToHeap (const uint8_t * data, uint16_t len) {
    uint8_t res = heap.add (data, len);
    if (res) CRemoteXYThread::notifyComplexVarNeedSend (guiData);
    return res;  
  }
  
  public:
  uint8_t addToHeap (const uint8_t * data1, uint16_t len1, const uint8_t * data2, uint16_t len2) {
    uint8_t res = heap.add (data1, len1, data2, len2);
    if (res) CRemoteXYThread::notifyComplexVarNeedSend (guiData);
    return res;  
  }
  

  public:
  uint8_t getDescriptorLength () override {
    return sizeof (CRemoteXYTypeInner_Heap_Descriptor);
  }
  
  public:
  void sendDescriptorBytes (CRemoteXYWire * wire) override {
    CRemoteXYTypeInner_Heap_Descriptor desc;
    desc.count = heap.count; 
    desc.lastId = heap.lastId; 
    wire->sendBytesPackage ((uint8_t*)(&desc), sizeof (CRemoteXYTypeInner_Heap_Descriptor));
  }
  
  public:
  uint8_t handlePackage (CRemoteXYPackage * package, CRemoteXYWire * wire) override {
    if (package->length != sizeof (CRemoteXYTypeInner_Heap_PackageHead)) return 0;
    CRemoteXYTypeInner_Heap_PackageHead * packageHead = (CRemoteXYTypeInner_Heap_PackageHead*)package->buffer;    
    uint16_t bytesLen = 0;
    
    if (heap.count > 0) {
      uint16_t firstId = packageHead->id;  
      if (!heap.containsId (firstId)) firstId = heap.firstId;   
      bytesLen = heap.takeBytes (firstId);
    }
    wire->startPackage (REMOTEXY_PACKAGE_COMMAND_COMPLEXDATA, package->clientId, bytesLen+2); 
    wire->sendBytesPackage (package->buffer, 2);  // num
    while (bytesLen--) {
      wire->sendBytePackage (heap.getNextByte ());
    } 
    return 1;
  }
  

};


#endif // RemoteXYType_Heap_h