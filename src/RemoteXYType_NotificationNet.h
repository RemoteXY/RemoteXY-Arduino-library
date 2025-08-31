#ifndef RemoteXYType_NotificationNet_h
#define RemoteXYType_NotificationNet_h

#include "RemoteXYType_Notification.h"
#include "RemoteXYNet.h"
#include "RemoteXYHttpRequest.h"

#include "RemoteXYAes.h"

#if defined(REMOTEXY_HAS_EEPROM)

#define REMOTEXY_NOTIFICATION_VERSION 1                      

const char REMOTEXY_NOTIFICATION_URL[] PROGMEM = "/notification";

#define REMOTEXY_NOTIFICATION_STATE_NO 0                      
#define REMOTEXY_NOTIFICATION_STATE_LATENCY 1                                          
#define REMOTEXY_NOTIFICATION_STATE_REQUEST 2                                           
                   
               
#define REMOTEXY_NOTIFICATION_LATENCY_TIME 60000  
                   


class CRemoteXYTypeInner_NotificationNet : public CRemoteXYTypeInner_Notification, CRemoteXYHttpRequestListener {
  CRemoteXYHttpRequest * httpRequest; // may be NULL  
  uint8_t httpRequestState;
  uint32_t httpRequestTimeout;
  uint8_t httpRequestAnswerBuffer[REMOTEXY_AES_BLOCK_SIZE];  // new aes iv
  uint16_t httpRequestHeapId; // next id to send
  
  public:
  uint8_t* init (uint8_t *conf) override  {
    httpRequest = NULL;
    httpRequestState = REMOTEXY_NOTIFICATION_STATE_NO;
    guiData->data->eeprom.createAesKeyItem ();
    httpRequestHeapId = heap.firstId;
    return CRemoteXYTypeInner_Notification::init (conf);
  };  
  

  void setState (uint8_t state) {
    httpRequestState = state; 
    if (state != REMOTEXY_NOTIFICATION_STATE_REQUEST) httpRequest = NULL;
    if (state == REMOTEXY_NOTIFICATION_STATE_LATENCY) httpRequestTimeout = millis ();
  }

  public: 
  void handler () override {    
    send ();
    
    RemoteXYStorageHeap_Head heapHead;
    CRemoteXYData *data = guiData->data;
    
    if (httpRequestState == REMOTEXY_NOTIFICATION_STATE_LATENCY) {
      if (millis () - httpRequestTimeout >= REMOTEXY_NOTIFICATION_LATENCY_TIME) {
        setState (REMOTEXY_NOTIFICATION_STATE_NO);
      }
    }

    if (httpRequest != NULL) {
      httpRequest->handler ();
    }
    else { 
      if (httpRequestState == REMOTEXY_NOTIFICATION_STATE_NO) {
        CRemoteXYEepromItem * boardId = data->eeprom.getBoardIdItem ();
        CRemoteXYEepromItem * aesKey = data->eeprom.getAesKeyItem ();
        if (boardId == NULL) return;
        if (aesKey == NULL) return;
        if (boardId->isEmpty ()) return;
        if (aesKey->isEmpty ()) return;
         
        if (!heap.findById (httpRequestHeapId, &heapHead)) {
          if (httpRequestHeapId - heap.firstId < 0x7fff) return; // no new messages
          httpRequestHeapId = heap.firstId; 
          if (!heap.findById (httpRequestHeapId, &heapHead)) return; // no messages
        }

        httpRequest = CRemoteXYHttpRequest::getHttpRequest (data->nets);
        if (httpRequest != NULL) {    
                
          uint16_t postDataLength = 1 + boardId->size;
          
          postDataLength += CRemoteXYAesCbcWriter::getEncriptDataSize (
            sizeof (int64_t) + 
            sizeof (RemoteXYStorageHeap_Head) + 
            heapHead.len
          );  
                  
          setState (REMOTEXY_NOTIFICATION_STATE_REQUEST);
          httpRequest->setListener (this);
          httpRequest->setAnswerBuffer (httpRequestAnswerBuffer, REMOTEXY_AES_BLOCK_SIZE);
          httpRequest->setRequest (
              FPSTR(REMOTEXY_HTTPREQUEST_HOST_REMOTEXY), 
              REMOTEXY_HTTPREQUEST_PORT_REMOTEXY, 
              FPSTR(REMOTEXY_NOTIFICATION_URL),
              FPSTR(REMOTEXY_HTTPREQUEST_METHOD_POST));
          httpRequest->setPostDataLength (postDataLength);    
#if defined(REMOTEXY__DEBUGLOG)
          RemoteXYDebugLog.write(F("Send Notification request..."));
#endif 
          httpRequest->send ();            
        }
      }
    }  
  }

  
  void httpRequestCompletion (uint8_t result) override {
    if (httpRequestState == REMOTEXY_NOTIFICATION_STATE_REQUEST) {
      if (result && (httpRequest != NULL))  {
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write(F("Notification request: OK"));
#endif 
        if (httpRequest->getContentLength () == REMOTEXY_AES_BLOCK_SIZE) {
          RemoteXYAesCbc.setIv (httpRequestAnswerBuffer);
          httpRequestHeapId++; 
          setState (REMOTEXY_NOTIFICATION_STATE_NO);   
          return;
        }
      }
    }
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("Notification request: ERROR"));
#endif 
    setState (REMOTEXY_NOTIFICATION_STATE_LATENCY);
  } 

  uint8_t httpRequestSendPostData (CRemoteXYClient * client) override {
    CRemoteXYData *data = guiData->data;
    CRemoteXYEepromItem * boardId = data->eeprom.getBoardIdItem ();
    CRemoteXYEepromItem * aesKey = data->eeprom.getAesKeyItem ();
    if ((boardId != NULL) && (aesKey != NULL)) {
      RemoteXYStorageHeap_Head heapHead;
      if (heap.findById (httpRequestHeapId, &heapHead)) {
        CRemoteXYAesCbcWriter aesWriter (client, &RemoteXYAesCbc);
        RemoteXYAesCbc.setKey (aesKey->data);
        
        // lib version
        client->write (REMOTEXY_NOTIFICATION_VERSION);
        // board id
        client->write (boardId->data, boardId->size);  
          
        // encript
        aesWriter.writeStart (sizeof (int64_t) + sizeof (RemoteXYStorageHeap_Head) + heapHead.len);
        // board time
        int64_t t = data->boardTime;  
        aesWriter.write ((uint8_t*)&t, sizeof (int64_t));        
        // heap head
        aesWriter.write ((uint8_t*)&heapHead, sizeof (RemoteXYStorageHeap_Head));     
        // message head and message      
        uint16_t len = heapHead.len;    
        while (len--) {
          aesWriter.write (heap.getNextByte());
        } 
        return 1;    
      } 
    }
    return 0;  // this message is no longer there, sorry                    
  }
   
};

#pragma pack(push, 1) 
class RemoteXYType_NotificationNet : public RemoteXYType_Notification {
  
  public:
  RemoteXYType_NotificationNet () {
    inner = new CRemoteXYTypeInner_NotificationNet ();
  } 
    
};

#pragma pack(pop)

#endif // REMOTEXY_HAS_EEPROM
#endif // RemoteXYType_NotificationNet_h