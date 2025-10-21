#ifndef RemoteXYType_Notification_h
#define RemoteXYType_Notification_h

#include "RemoteXYData.h"
#include "RemoteXYType_HeapPrintable.h"
                   

#pragma pack(push, 1)
struct RemoteXYType_Notification_Head {
  int64_t time;
};
#pragma pack(pop)

class CRemoteXYTypeInner_Notification : public CRemoteXYTypeInner_HeapPrintable { 

  public:
  uint8_t* init (uint8_t *conf) override  {
    return CRemoteXYTypeInner_HeapPrintable::init (conf);
  }; 
  
  
  void handler () override {      
    send ();
  };   
 
  public:   
  uint8_t addBufferToHeap (uint8_t *buf, uint16_t len) override {
    RemoteXYType_Notification_Head head;
    head.time = guiData->data->boardTime;
    return addToHeap ((uint8_t*)&head, sizeof(RemoteXYType_Notification_Head), buf, len);
  }  
  

   
};

#define CRemoteXYTypeInner_Notification_inner ((CRemoteXYTypeInner_Notification*)inner)

#pragma pack(push, 1) 
class RemoteXYType_Notification : public CRemoteXYType_HeapPrintable {
  
  public:
  RemoteXYType_Notification () {
    inner = new CRemoteXYTypeInner_Notification ();
  } 
    
};

#pragma pack(pop)


#endif // RemoteXYType_Notification_h