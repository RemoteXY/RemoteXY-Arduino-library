#ifndef RemoteXYType_NotificationNet_h
#define RemoteXYType_NotificationNet_h

#include "RemoteXYType_Notification.h"
#include "RemoteXYNet.h"
#include "RemoteXYServerRequest.h"

#include "RemoteXYAes.h"

#if defined(REMOTEXY_HAS_EEPROM)

#define REMOTEXY_NOTIFICATION_VERSION 1                      

const char REMOTEXY_NOTIFICATION_COMMAND[] PROGMEM = "notification";
const char REMOTEXY_NOTIFICATION_ANSWER_OK[] PROGMEM = "ok";

#define REMOTEXY_NOTIFICATION_COMMAND_GET 0x01                      
#define REMOTEXY_NOTIFICATION_COMMAND_CONFIRM 0x02                      


#define REMOTEXY_NOTIFICATION_STATE_NO 0
#define REMOTEXY_NOTIFICATION_STATE_LATENCY 1                                          
#define REMOTEXY_NOTIFICATION_STATE_SERVER_MISSING 2                      
#define REMOTEXY_NOTIFICATION_STATE_SERVER_ERROR 3                                          
#define REMOTEXY_NOTIFICATION_STATE_WAIT_APP 4                                          
#define REMOTEXY_NOTIFICATION_STATE_WAIT_APP_CONFIRM 5                                          
#define REMOTEXY_NOTIFICATION_STATE_RECEIVE_IV 6                                                                                     
#define REMOTEXY_NOTIFICATION_STATE_SEND_NOTIFICATION 7 
#define REMOTEXY_NOTIFICATION_STATE_RECEIVE_NOTIFICATION 8                                           
                   
               
#define REMOTEXY_NOTIFICATION_APP_TIMEOUT_MS 10000  
#define REMOTEXY_NOTIFICATION_MESSAGE_LATENCY_MS 300000  
#define REMOTEXY_NOTIFICATION_ERROR_LATENCY_MS 60000  

#define REMOTEXY_NOTIFICATION_RECEIVE_BUFFER_SIZE REMOTEXY_AES_BLOCK_SIZE
                   
#pragma pack(push, 1)

struct RemoteXYType_NotificationNet_Head {
  int64_t time;
};
         
struct CRemoteXYTypeInner_NotificationNet_PackageHead {
  uint16_t num;
  uint8_t command;
};

#pragma pack(pop)


class CRemoteXYTypeInner_NotificationNet : public CRemoteXYTypeInner_HeapPrintable, CRemoteXYServerRequestListener {
  CRemoteXYServerRequest * serverRequest; // may be NULL   
  uint8_t sendState;
  uint32_t timeout;
  uint32_t starttime;
  uint8_t requestReceiveBuffer[REMOTEXY_NOTIFICATION_RECEIVE_BUFFER_SIZE]; 
  CRemoteXYEepromItem * boardId;
  CRemoteXYEepromItem * aesKey;
  
  uint16_t sendHeapId; // next id to send  

  
  public:
  uint8_t* init (uint8_t *conf) override  {
    CRemoteXYData *data = guiData->data;
    serverRequest = NULL;
    sendState = REMOTEXY_NOTIFICATION_STATE_NO;
    timeout = 0;
    data->eeprom.createBoardIdItem ();
    data->eeprom.createAesKeyItem ();    
    boardId = data->eeprom.getBoardIdItem ();
    aesKey = data->eeprom.getAesKeyItem ();
    sendHeapId = heap.firstId;
    return CRemoteXYTypeInner_HeapPrintable::init (conf);
  };    
  

  
  private:
  uint8_t checkNeedSend (RemoteXYStorageHeap_Head * heapHead) {    
    if ((boardId == NULL) || (aesKey == NULL)) return 0;
    if (boardId->isEmpty ()) return 0;
    if (aesKey->isEmpty ()) return 0;
    if (!heap.findById (sendHeapId, heapHead)) {
      if (sendHeapId - heap.firstId < 0x7fff) return 0; // no new messages
      sendHeapId = heap.firstId; 
      if (!heap.findById (sendHeapId, heapHead)) return 0; // no messages
    }
    return 1;
  }  
  
  private:
  void setState (uint8_t state) {
    if (state != sendState) {
      sendState = state;
      if ((state == REMOTEXY_NOTIFICATION_STATE_SERVER_MISSING) || (state == REMOTEXY_NOTIFICATION_STATE_SERVER_ERROR)) {
        RemoteXYStorageHeap_Head heapHead;
        if (checkNeedSend (&heapHead)) {
          CRemoteXYThread::notifyComplexVarNeedSend (guiData);
        }
      }
      starttime = millis ();
      if (state == REMOTEXY_NOTIFICATION_STATE_SERVER_ERROR) {
        timeout = REMOTEXY_NOTIFICATION_ERROR_LATENCY_MS;
      }
      else if (state == REMOTEXY_NOTIFICATION_STATE_LATENCY) {
        timeout = REMOTEXY_NOTIFICATION_MESSAGE_LATENCY_MS;
      }
      else if ((state == REMOTEXY_NOTIFICATION_STATE_WAIT_APP) || (sendState == REMOTEXY_NOTIFICATION_STATE_WAIT_APP_CONFIRM)) {
        timeout = REMOTEXY_NOTIFICATION_APP_TIMEOUT_MS;
      }  
      else {
        timeout = 0;
      } 
    }
  }
  
  private:
  void sendMessageData (RemoteXYStorageHeap_Head * heapHead, CRemoteXYOutput * output) {
    // board time
    int64_t t = guiData->data->boardTime;  
    output->write ((uint8_t*)&t, sizeof (int64_t));        
    // heap head
    output->write ((uint8_t*)heapHead, sizeof (RemoteXYStorageHeap_Head));     
    // message head and message      
    uint16_t len = heapHead->len;    
    while (len--) {
      output->write (heap.getNextByte());
    } 
  }     
  
  private:
  uint16_t getMessageDataSize (RemoteXYStorageHeap_Head * heapHead) {
    return sizeof (int64_t) + sizeof (RemoteXYStorageHeap_Head) + heapHead->len;
  }         
    
  public:
  uint8_t getDescriptorLength () override {
    return 1;
  }          
  
  public:
  void sendDescriptorBytes (CRemoteXYWire * wire) override {
    // server in priority
    if ((sendState == REMOTEXY_NOTIFICATION_STATE_SERVER_MISSING) || (sendState == REMOTEXY_NOTIFICATION_STATE_SERVER_ERROR)) {
      RemoteXYStorageHeap_Head heapHead;
      if (checkNeedSend (&heapHead)) {
        setState (REMOTEXY_NOTIFICATION_STATE_WAIT_APP);
        wire->write (1);
        return;
      }
    }
    wire->write (0);
  } 
    
  public:
  uint8_t handlePackage (CRemoteXYPackage * package, CRemoteXYWire * wire) override {  
    if (package->length < sizeof (CRemoteXYTypeInner_NotificationNet_PackageHead)) return 0;
    CRemoteXYTypeInner_NotificationNet_PackageHead * packageHead = (CRemoteXYTypeInner_NotificationNet_PackageHead*)package->buffer;    
    RemoteXYStorageHeap_Head heapHead;
    if (packageHead->command == REMOTEXY_NOTIFICATION_COMMAND_GET) {
      if (sendState == REMOTEXY_NOTIFICATION_STATE_WAIT_APP) {
        if (checkNeedSend (&heapHead)) {
          if (package->length != sizeof (CRemoteXYTypeInner_NotificationNet_PackageHead) + REMOTEXY_AES_BLOCK_SIZE) return 0;
          uint16_t dataSize = getMessageDataSize (&heapHead);
          wire->startPackage (REMOTEXY_PACKAGE_COMMAND_COMPLEXDATA, package->clientId, 2 + CRemoteXYAesCbcWriter::getEncriptDataSize (dataSize));      
          wire->write ((uint8_t*)&packageHead->num, 2);  // num
          CRemoteXYAesCbcWriter aesWriter (wire, &RemoteXYAesCbc);
          RemoteXYAesCbc.setKey (aesKey->data);
          RemoteXYAesCbc.setIv ((uint8_t*)package->buffer + sizeof (CRemoteXYTypeInner_NotificationNet_PackageHead));   
          aesWriter.startWrite (dataSize);                 
          sendMessageData (&heapHead, &aesWriter);
          setState (REMOTEXY_NOTIFICATION_STATE_WAIT_APP_CONFIRM);
          return 1;
        }   
      }
    }      
    else if (packageHead->command == REMOTEXY_NOTIFICATION_COMMAND_CONFIRM) {
      if (sendState == REMOTEXY_NOTIFICATION_STATE_WAIT_APP_CONFIRM) {
        sendHeapId++; 
        wire->startPackage (REMOTEXY_PACKAGE_COMMAND_COMPLEXDATA, package->clientId, 2);      
        wire->write ((uint8_t*)&packageHead->num, 2);  // num
        setState (REMOTEXY_NOTIFICATION_STATE_LATENCY);
        return 1;
      }
    }

    return 0;
  }  

  public: 
  void handler () override {    
    send ();
    
    RemoteXYStorageHeap_Head heapHead;
    
    if (timeout != 0) {
      if (millis () - starttime >= timeout) {
        setState (REMOTEXY_NOTIFICATION_STATE_NO);
      }
    }

    if (serverRequest != NULL) {
      serverRequest->handler ();
      
      if (sendState == REMOTEXY_NOTIFICATION_STATE_SEND_NOTIFICATION) {
        if (heap.findById (sendHeapId, &heapHead)) {
          setState (REMOTEXY_NOTIFICATION_STATE_RECEIVE_NOTIFICATION); 
          serverRequest->send (FPSTR(REMOTEXY_NOTIFICATION_COMMAND), getDataLength (&heapHead));  
        }      
      }    
    }
    else { 
      if ((sendState == REMOTEXY_NOTIFICATION_STATE_NO) || (sendState == REMOTEXY_NOTIFICATION_STATE_SERVER_MISSING)) {
        if (checkNeedSend (&heapHead)) {
          serverRequest = CRemoteXYServerRequest::getServerRequest (guiData->data->nets);
          if (serverRequest != NULL) { 
            serverRequest->setListener (this);
            serverRequest->setReceiveBuffer (requestReceiveBuffer, REMOTEXY_NOTIFICATION_RECEIVE_BUFFER_SIZE);
            setState (REMOTEXY_NOTIFICATION_STATE_RECEIVE_IV);
#if defined(REMOTEXY__DEBUGLOG)
            RemoteXYDebugLog.init ();
            RemoteXYDebugLog.write(F("Send Notification request..."));
#endif 
            serverRequest->send (FPSTR(REMOTEXY_SERVERREQUEST_COMMAND_AESIV), 0);
          }
          else { 
            setState (REMOTEXY_NOTIFICATION_STATE_SERVER_MISSING);
          }
        }
      }
    }  
  }

  private:
  void closeServerRequest () {
    serverRequest->close ();
    serverRequest = NULL;
  }       
    
  void serverRequestCompletion (uint8_t result) override {
    if (serverRequest != NULL) {
      if (result) {
        if (sendState == REMOTEXY_NOTIFICATION_STATE_RECEIVE_IV) {
          if (serverRequest->getReceiveDataLength () == REMOTEXY_AES_BLOCK_SIZE) {
            setState (REMOTEXY_NOTIFICATION_STATE_SEND_NOTIFICATION); 
            return;
          }
        }
        else if (sendState == REMOTEXY_NOTIFICATION_STATE_RECEIVE_NOTIFICATION) {
          uint8_t len = serverRequest->getReceiveDataLength ();
          if (len < REMOTEXY_NOTIFICATION_RECEIVE_BUFFER_SIZE) {
            requestReceiveBuffer[len] = 0;
            if (rxy_strCompare ((char*)requestReceiveBuffer, FPSTR(REMOTEXY_NOTIFICATION_ANSWER_OK), 1)) {
              sendHeapId++; 
#if defined(REMOTEXY__DEBUGLOG)
              RemoteXYDebugLog.write(F("Notification request: OK"));
#endif  
              closeServerRequest ();
              setState (REMOTEXY_NOTIFICATION_STATE_LATENCY);  
              return;
            }
          }
        }
      }
    }
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("Notification request: ERROR"));
#endif 
    closeServerRequest (); 
    setState (REMOTEXY_NOTIFICATION_STATE_SERVER_ERROR);
  } 


  private:
  uint16_t getDataLength (RemoteXYStorageHeap_Head * heapHead) {    
    return 1 + boardId->size + CRemoteXYAesCbcWriter::getEncriptDataSize (getMessageDataSize (heapHead));  
  }  


  uint8_t serverRequestSendData (CRemoteXYClient * client) override {
    RemoteXYStorageHeap_Head heapHead;
    if (heap.findById (sendHeapId, &heapHead)) {
    
      
      // lib version
      client->write (REMOTEXY_NOTIFICATION_VERSION);
      // board id
      client->write (boardId->data, boardId->size);  
        
      // encript
      CRemoteXYAesCbcWriter aesWriter (client, &RemoteXYAesCbc);
      RemoteXYAesCbc.setKey (aesKey->data);
      RemoteXYAesCbc.setIv (requestReceiveBuffer);
      aesWriter.startWrite (getMessageDataSize (&heapHead));
      sendMessageData (&heapHead, &aesWriter);
      return 1;    
    }
    return 0;  // this message is no longer there, sorry                    
  }
  
  public:   
  uint8_t addBufferToHeap (uint8_t *buf, uint16_t len) override {
    RemoteXYType_NotificationNet_Head head;
    head.time = guiData->data->boardTime;
    return addToHeap ((uint8_t*)&head, sizeof(RemoteXYType_NotificationNet_Head), buf, len);
  }  
   
};

#define CRemoteXYTypeInner_NotificationNet_inner ((CRemoteXYTypeInner_NotificationNet*)inner)

#pragma pack(push, 1) 
class RemoteXYType_NotificationNet : public CRemoteXYType_HeapPrintable {
  
  public:
  RemoteXYType_NotificationNet () {
    inner = new CRemoteXYTypeInner_NotificationNet ();
  } 
  
    
};

#pragma pack(pop)

#endif // REMOTEXY_HAS_EEPROM
#endif // RemoteXYType_NotificationNet_h