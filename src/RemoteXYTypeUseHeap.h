#ifndef RemoteXYTypeUseHeap_h
#define RemoteXYTypeUseHeap_h

#include "RemoteXYStorageHeap.h"
#include "RemoteXYType.h"
#include "RemoteXYThread.h"

#pragma pack(push, 1)
struct CRemoteXYTypeUseHeap_Descriptor {
  uint16_t count;
  uint16_t firstId;
  uint16_t lastId;
};

struct CRemoteXYTypeUseHeap_PackageHead {
  uint16_t num;
  uint16_t id;
};
#pragma pack(pop)


class CRemoteXYTypeUseHeap : public CRemoteXYType {
  protected:
  CRemoteXYStorageHeap heap;
  
  public:
  uint8_t* init (uint8_t *conf) override  {
    uint16_t b = rxy_readConfByte (conf++);
    heap.init (b<<6);    
    return conf;
  };
  
  protected:
  uint8_t addToHeap (uint8_t * data, uint16_t len) {
    uint8_t res = heap.add (data, len);
    if (res) CRemoteXYThread::notifyComplexVarNeedSend (guiData);
    return res;  
  }
  
  protected:
  uint8_t addToHeap (uint8_t * data1, uint16_t len1, uint8_t * data2, uint16_t len2) {
    uint8_t res = heap.add (data1, len1, data2, len2);
    if (res) CRemoteXYThread::notifyComplexVarNeedSend (guiData);
    return res;  
  }

  public:
  uint16_t getDescriptorLength () override {
    return sizeof (CRemoteXYTypeUseHeap_Descriptor);
  }

  public:
  void getDescriptor (uint8_t * buf) override { 
    RemoteXYStorageHeap_Head heapHead;
    CRemoteXYTypeUseHeap_Descriptor * desc = (CRemoteXYTypeUseHeap_Descriptor*)buf;
    desc->count = heap.count;
    if (heap.getFirst (&heapHead)) {
      desc->firstId = heapHead.id;
    }
    else {
      desc->firstId = heap.lastId + 1;
    }
    desc->lastId = heap.lastId;
  }
  
  public:
  uint8_t receivePackage (CRemoteXYPackage * package, CRemoteXYWire * wire) override {
    if (package->length != sizeof (CRemoteXYTypeUseHeap_PackageHead)) return 0;
    CRemoteXYTypeUseHeap_PackageHead * head = (CRemoteXYTypeUseHeap_PackageHead*)package->buffer;
    RemoteXYStorageHeap_Head heapHead;  
    uint16_t packageLen = sizeof (CRemoteXYTypeUseHeap_PackageHead);    
    uint8_t has = heap.findById (head->id, &heapHead);
    if (has) packageLen += heapHead.len;
    wire->startPackage (package->command, package->clientId, packageLen); 
    wire->sendBytesPackage (package->buffer, sizeof (CRemoteXYTypeUseHeap_PackageHead));
    if (has) {
      for (uint16_t i = 0; i < heapHead.len; i++) {
        wire->sendBytePackage (heap.getNextByte ());
      } 
    }
    wire->endPackage ();
    return 1;
  }
  

};




#endif // RemoteXYTypeUseHeap_h