#ifndef RemoteXYType_ServerRequest_h
#define RemoteXYType_ServerRequest_h


#define REMOTEXY_SERVERREQUEST_TIMEOUT 20000 


const char REMOTEXY_SERVERREQUEST_HOST[] PROGMEM = "boardproxy.remotexy.com"; // "192.168.3.4"; 
#define REMOTEXY_SERVERREQUEST_PORT_REMOTEXY 6374                     

const char REMOTEXY_SERVERREQUEST_COMMAND_CLOSE[] PROGMEM = "close";
const char REMOTEXY_SERVERREQUEST_COMMAND_AESIV[] PROGMEM = "aesiv";


#define REMOTEXY_SERVERREQUEST_UNUSED 0
#define REMOTEXY_SERVERREQUEST_USED 1
#define REMOTEXY_SERVERREQUEST_CONNECTING 2
#define REMOTEXY_SERVERREQUEST_READDATA 3
#define REMOTEXY_SERVERREQUEST_OK 4
#define REMOTEXY_SERVERREQUEST_ERROR 5

class CRemoteXYServerRequest;
             
class CRemoteXYServerRequestListener {
  public:
  virtual void serverRequestCompletion (uint8_t result) = 0;
  virtual uint8_t serverRequestSendData (CRemoteXYClient * client) {UNUSED (client); return 0;};
};

class CRemoteXYServerRequest: public CRemoteXYReadByteListener {
  public:
  CRemoteXYNet * net;
  
  private:
  CRemoteXYServerRequestListener * listener; 
  CRemoteXYClient * client;
  uint8_t state;

  const __FlashStringHelper * command;
  uint16_t sendDataLength;
    
  uint32_t timeOut;
  
  uint8_t * receiveBuf;
  uint16_t receiveBufSize;
  uint16_t receiveCounter;
  uint16_t receiveDataLength;  // real length 

  public:
  CRemoteXYServerRequest (CRemoteXYNet * _net) {
    net = _net;
    client = _net->newClient();
    listener = NULL;
    state = REMOTEXY_SERVERREQUEST_UNUSED;
  }
  
  public:
  void setListener (CRemoteXYServerRequestListener * _listener) {
    listener = _listener;
  }  
  
  public:
  void notifyCompletion (uint8_t result) {
    if (result) state = REMOTEXY_SERVERREQUEST_OK;
    else state = REMOTEXY_SERVERREQUEST_ERROR;
    if (listener) listener->serverRequestCompletion (result);
  }  
  
  void close () {
    if (client->connected()) {
      client->write (FPSTR(REMOTEXY_SERVERREQUEST_COMMAND_CLOSE));
      client->write (0x0d); 
      client->flush();
      client->stop ();
    }
    state = REMOTEXY_SERVERREQUEST_UNUSED;
  }
  
  private:
  uint8_t isUnused () {
    if (state == REMOTEXY_SERVERREQUEST_UNUSED) return 1;
    return 0;
  }
  
  private:
  void setUsed () {
    receiveBufSize = 0;
    state = REMOTEXY_SERVERREQUEST_USED;
  }  
  
  public:
  uint16_t getReceiveDataLength () {
    return receiveDataLength;
  }
       
  
  public:
  void setReceiveBuffer (uint8_t * buf, uint16_t size) {
    receiveBuf = buf;
    receiveBufSize = size;
  }  
 
  public:
  void send (const __FlashStringHelper * _command, uint16_t _sendDataLength) {
    command = _command;
    sendDataLength = _sendDataLength;  

    if (client->connected ()) {
      sendData ();
    }
    else {
      state = REMOTEXY_SERVERREQUEST_CONNECTING; 
      if (client->connect (FPSTR(REMOTEXY_SERVERREQUEST_HOST), REMOTEXY_SERVERREQUEST_PORT_REMOTEXY)) {
        client->setReadByteListener (this);
        timeOut = millis();      
      }
      else {
        notifyCompletion (0);
      }
    }

  }
  
  private:
  void sendData () {
    timeOut = millis();
    state = REMOTEXY_SERVERREQUEST_READDATA;
    receiveDataLength = 0;
    receiveCounter = 0;
    client->write (command);
    client->write (0x0d); 
    client->write (sendDataLength);
    client->write (sendDataLength >> 8);   
    if (sendDataLength > 0) {
      if (listener) {
        if (listener->serverRequestSendData (client) == 0) {
          notifyCompletion (0); 
        }
      }
    }    
    client->flush ();       
  }
 
   
  
  public:
  void handler () {
    if ((state == REMOTEXY_SERVERREQUEST_UNUSED) || (state == REMOTEXY_SERVERREQUEST_USED)) return;
    client->handler ();    
    
    if (state == REMOTEXY_SERVERREQUEST_CONNECTING) {
      if (client->connected ()) {
        sendData ();
      }   
    }    
    
    if (state == REMOTEXY_SERVERREQUEST_READDATA) {
      if (client->connected () == 0) {
        notifyCompletion (0);
        return;
      }
    }

    // check timout
    if (millis () - timeOut >  REMOTEXY_SERVERREQUEST_TIMEOUT) {
      notifyCompletion (0);
    }
        
  }
  
  public:
  void readByte (uint8_t byte) override { 
    if (state == REMOTEXY_SERVERREQUEST_READDATA) {
      if (receiveCounter == 0) {
        receiveDataLength = byte; 
      }
      else if (receiveCounter == 1) {
        receiveDataLength |= ((uint16_t)byte) << 8;        
        if (receiveDataLength == 0) {
          notifyCompletion (1);          
        }
      }
      else {
        uint16_t i = receiveCounter-2;
        if (i < receiveBufSize) {
          receiveBuf[i] = byte;
        }
        i++;
        if (i >= receiveDataLength) {
          notifyCompletion (1);   
        }
      }
      receiveCounter++;
    }      
  }  
     
   
  public:
  static CRemoteXYServerRequest * getServerRequest (CRemoteXYNet * nets) {
    CRemoteXYNet * net = nets;
    while (net) {
      if (net->configured ()) {
        if (net->serverRequest == NULL) {
          if (net->hasInternet ()) {
            net->serverRequest = new CRemoteXYServerRequest (net); 
          }  
        }
        if (net->serverRequest != NULL) {
          if (net->serverRequest->isUnused ()) {
            net->serverRequest->setUsed ();
            return net->serverRequest;
          }
        }
      }
      net = net->next;
    }    
    return NULL; 
  }  
  
};

  
#endif //RemoteXYType_ServerRequest_h