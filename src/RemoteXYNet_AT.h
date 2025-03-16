#ifndef RemoteXYNet_AT_h
#define RemoteXYNet_AT_h

#include "RemoteXYDebugLog.h"
#include "RemoteXYNet.h"
#include "RemoteXYFunc.h"
#include <stdarg.h>

#define UNUSED(x) (void)(x)

#define REMOREXYNET_AT__RECEIVE_BUFFER_SIZE 32 // maximum response length

#define REMOREXYNET_AT__COMMAND_TIMEOUT 1000
#define REMOREXYNET_AT__SEND_TIMEOUT 5000
#define REMOREXYNET_AT__TEST_TIMEOUT 30000
#define REMOREXYNET_AT__CONNECT_TIMEOUT 20000
#define REMOREXYNET_AT__NO_CHANNEL 0xff


#define AT_ID_BLOCKING 255
#define AT_ID_FINDMODULE 254
#define AT_ID_TESTMODULE 253

#define AT_RESULT_OK 1
#define AT_RESULT_TIMEOUT 2
#define AT_RESULT_ERROR 3
#define AT_RESULT_FAIL 4
#define AT_RESULT_BUSY 5
#define AT_RESULT_RESET 6
#define AT_RESULT_SEND_READY 7
#define AT_RESULT_SEND_OK 8
#define AT_RESULT_SEND_FAIL 9
#define AT_RESULT_MESSAGE 10

// standards answers
const char * AT_ANSWER_ERROR = "ERROR";
const char * AT_ANSWER_FAIL = "FAIL";
const char * AT_ANSWER_BUSY = "BUSY *";
const char * AT_ANSWER_OK = "OK";
const char * AT_ANSWER_SEND_OK = "SEND OK";
const char * AT_ANSWER_SEND_FAIL = "SEND FAIL";
const char * AT_MESSAGE_READY = "READY";
const char * AT_MESSAGE_AT = "AT";
const char * AT_MESSAGE_CONNECT = "?,CONNECT";
const char * AT_MESSAGE_CLOSED = "?,CLOSED";
const char * AT_MESSAGE_CONNECT_FAIL = "?,CONNECT FAIL";
const char * AT_MESSAGE_IPD = "+IPD,?,*:";
const char * AT_MESSAGE_CIPSEND = "AT+CIPSEND=";
const char * AT_MESSAGE_CIPCLOSE = "AT+CIPCLOSE=";
const char * AT_MESSAGE_CIPSTART = "AT+CIPSTART=";


class CRemoteXYNet_AT_ReadByteListener {
  public:
  virtual void readByte (uint8_t byte) = 0;
};


class CRemoteXYNet_AT_Client {

  public:
  CRemoteXYNet_AT_Client * next;
  uint8_t id;
  uint8_t connected;
  CRemoteXYNet_AT_ReadByteListener * readByteListener;
  
  public:
  CRemoteXYNet_AT_Client () {
    connected = 0;
    readByteListener = NULL;
  }
  
  
  public:
  void setReadByteListener (CRemoteXYNet_AT_ReadByteListener * listener) {
    readByteListener = listener;
  } 
  
  public:   
  void notifyReadByteListener (uint8_t byte) {
    if (readByteListener && connected) readByteListener->readByte (byte);
  }
};

class CRemoteXYNet_AT_ClientAvalaibleListener {
  public:
  virtual void clientAvailable (CRemoteXYNet_AT_Client * client) = 0;
};
             

class CRemoteXYNet_AT : public CRemoteXYNet, public CRemoteXYReadByteListener {

  private:
  CRemoteXYStream * stream;

  public:
  CRemoteXYNet_AT_ClientAvalaibleListener * clientAvalaibleListener;
  CRemoteXYNet_AT_Client * clients;
  
  
  protected:
  char receiveBuffer[REMOREXYNET_AT__RECEIVE_BUFFER_SIZE];
  uint8_t receiveBufferIndex;
  
  char * params[3];
  uint8_t paramsLength[3];      
  
  uint32_t lastAnswerTime;

  
  uint16_t connectAvailable;
  uint16_t freeAvailable;
  
  
  
  uint32_t commandTimeOut;
  uint32_t commandDelay;
  uint8_t commandIdentifier;
  void (*commandListener)(uint8_t, uint8_t);
  volatile uint8_t commandResult;
  volatile uint8_t commandBlocking;
  uint8_t commandFlag;
  
  uint16_t ipdId;
  uint16_t ipdSize;
  CRemoteXYNet_AT_Client * ipdClient;
  
  
  uint8_t findModuleTryCount;
  uint8_t haveEcho;
  
  uint16_t sendByteMax;
  uint16_t sendByteSize;
  uint16_t sendByteNext;
  
  uint8_t clientConnectingID;
  uint8_t detected;

  public:
  virtual void moduleReset () {};  
  virtual void moduleLost () {};  
  virtual void moduleFound () {};  
  virtual uint8_t handleATMessage () {return 0;}; 
  virtual void commandATListener (uint8_t identifier, uint8_t result) { UNUSED(identifier); UNUSED(result);};  

  

  
  
  public:
  CRemoteXYNet_AT (CRemoteXYStream *_stream, uint16_t _sendByteMax) : CRemoteXYNet () {
    stream = _stream;
    stream->setReadByteListener (this);
    sendByteMax = _sendByteMax;
    receiveBufferIndex=0;
    
    ipdSize = 0;
    lastAnswerTime = millis ();
    
    clientAvalaibleListener = NULL;
    clients = NULL;
    clientConnectingID = REMOREXYNET_AT__NO_CHANNEL;
    detected = 0;    // ???
    
    
    commandDelay = 0;
    commandIdentifier = 0;
    commandListener = NULL;
    findModuleTryCount = 0;
    commandBlocking = 0;
  }
  

  
  private:
  void resultATcommand (uint8_t identifier, uint8_t result) {
    uint8_t id = identifier;
    if (result !=AT_RESULT_MESSAGE) {
      commandResult = result;
      commandDelay = 0;
      commandIdentifier = 0;
    }
    
    if (id == AT_ID_FINDMODULE) {
      if (result == AT_RESULT_OK) {
        findModuleTryCount = 0;
        moduleFound ();
      }
    }
    else if (id == AT_ID_TESTMODULE) {
      if (result == AT_RESULT_MESSAGE) {
        if (strcmpReceiveBuffer (AT_MESSAGE_AT)==0) haveEcho = 1;
      }
      else if (result == AT_RESULT_OK) {
        if (haveEcho) moduleLost ();
        
      }
      else moduleLost ();
    }
    else {
      commandATListener (id, result);
      if (commandListener) commandListener (id, result);
    }
  }
  
    
  private:
  void streamWrite (const char *p) {
     while (*p) {
       stream->write (*p++); 
    }
  }  
  
  
  public:
  uint8_t readyATCommand () {
    if ((commandIdentifier) || (ipdSize) || (commandBlocking)) return 0;
    return 1;
  }
  
  public:
  void setATCommandListener (void (*listener)(uint8_t, uint8_t)) {
    commandListener = listener;
  }
  
  public:
  void setATTimeOut (uint32_t timeOut) {
    commandDelay = timeOut;
  }
  
  public:
  uint8_t getATResult () {
    return commandResult;
  }

 
  private:  
  void sendArgPtr (const char * p, va_list *argptr) { 
    if (p) {
      while (p) {
        streamWrite (p);      
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.writeOutput (p);
#endif     
        p=va_arg(*argptr,char*);
      }        
      streamWrite ("\r\n");
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.writeInputNewString ();
#endif  
    }
    commandTimeOut = millis();   
    if (commandDelay == 0) commandDelay = REMOREXYNET_AT__COMMAND_TIMEOUT;
    commandResult = 0;
  }
  
  // not blocking funktion, use:
  // for children class - virtual commandATListener (identifier, result)
  // for static function - setATListener (func)
  // result:
  //   0 - command did not send
  //   1 - command sent
  public:  
  uint8_t sendATCommand (uint8_t identifier, const char * command, ...) { 
    if (!readyATCommand ()) {
      commandDelay = 0;
      return 0;    
    }
    va_list argptr;
    va_start (argptr, command);
    sendArgPtr (command, &argptr);
    va_end(argptr);   
    commandIdentifier = identifier;   
    return 1;       
  }
  
  
  // blocking funktion
  // result: AT_RESULT_XXXXX
  public:  
  uint8_t sendATCommandForResult (const char * command, ...) { 
    if (!readyATCommand ()) {
      commandDelay = 0;
      return AT_RESULT_BUSY;
    }   
    va_list argptr;
    va_start (argptr, command);
    sendArgPtr (command, &argptr);
    va_end(argptr);   

    commandIdentifier = AT_ID_BLOCKING;
    commandBlocking = 1;   
    while (commandResult == 0) {
      if (millis() - commandTimeOut > commandDelay) {
        resultATcommand (commandIdentifier, AT_RESULT_TIMEOUT);
      }
      else stream->handler ();
    }   
    commandBlocking = 0;    
    return commandResult;       
  }
  
  
  public:  
  uint8_t waitATResult () { 
    return sendATCommandForResult (NULL, NULL);
  }
  
    
  public:
  void readByte (uint8_t byte) override {
          
    if (ipdSize) {
      ipdSize--;
      if (ipdClient) {
        ipdClient->notifyReadByteListener (byte);
      }          
#if defined(REMOTEXY__DEBUGLOG)
      if (ipdSize == 0) RemoteXYDebugLog.writeInputNewString ();
#endif      
      return;
    }
   
    if (byte==10) return;
#if defined(REMOTEXY__DEBUGLOG)
    if (byte==13) RemoteXYDebugLog.writeInputNewString ();
    else RemoteXYDebugLog.writeInputChar (byte);
#endif      
    if (byte==13) {
      lastAnswerTime = millis ();
      receiveBuffer[receiveBufferIndex]=0;
      if (receiveBufferIndex == 0) return;
      receiveBufferIndex=0;
      
      if (strcmpReceiveBuffer (AT_MESSAGE_CONNECT)==0) {   // new client
        uint8_t id = getATParamInt(0);
        if (id != clientConnectingID) {
          
          CRemoteXYNet_AT_Client * client = getUnusedClient ();
          client->id = id;
          client->connected = 1;
          if (clientAvalaibleListener) clientAvalaibleListener->clientAvailable (client);
        }
        return; 
      }               
      if (strcmpReceiveBuffer (AT_MESSAGE_CLOSED)==0) { clientClosed (); return; }   // client closed
      if (strcmpReceiveBuffer (AT_MESSAGE_CONNECT_FAIL)==0) { clientClosed (); return; }   // client closed
      if (strcmpReceiveBuffer (AT_MESSAGE_READY)==0) { 
        if (commandIdentifier) resultATcommand (commandIdentifier, AT_RESULT_TIMEOUT); 
        moduleReset (); 
        return; 
      }
      if (handleATMessage ()) return;
      if (commandIdentifier) {
        if (strcmpReceiveBuffer (AT_ANSWER_OK)==0) resultATcommand (commandIdentifier, AT_RESULT_OK); 
        else if (strcmpReceiveBuffer (AT_ANSWER_SEND_OK)==0) resultATcommand (commandIdentifier, AT_RESULT_SEND_OK); 
        else if (strcmpReceiveBuffer (AT_ANSWER_ERROR)==0) resultATcommand (commandIdentifier, AT_RESULT_ERROR); 
        else if (strcmpReceiveBuffer (AT_ANSWER_FAIL)==0) resultATcommand (commandIdentifier, AT_RESULT_FAIL); 
        else if (strcmpReceiveBuffer (AT_ANSWER_BUSY)==0) resultATcommand (commandIdentifier, AT_RESULT_BUSY); 
        else if (strcmpReceiveBuffer (AT_ANSWER_SEND_FAIL)==0) resultATcommand (commandIdentifier, AT_RESULT_SEND_FAIL); 
        else resultATcommand (commandIdentifier, AT_RESULT_MESSAGE);
        return;
      }         
    }
    else {
      if ((byte>0x60) && (byte<=0x7a)) byte-=0x20;
      if (receiveBufferIndex<REMOREXYNET_AT__RECEIVE_BUFFER_SIZE-1) receiveBuffer[receiveBufferIndex++]=byte;  
      if (byte==':') {
        lastAnswerTime = millis ();
        receiveBuffer[receiveBufferIndex]=0;
        receiveBufferIndex=0;
        if (strcmpReceiveBuffer (AT_MESSAGE_IPD)==0) {
#if defined(REMOTEXY__DEBUGLOG)
          RemoteXYDebugLog.writeInputNewString ();
#endif
          ipdId = getATParamInt(0); 
          ipdSize = getATParamInt(1);
          CRemoteXYNet_AT_Client * p = clients;
          while (p) {
            if ((p->id == ipdId) && (p->connected)) break;
            p = p->next;
          }
          ipdClient = p;
          return;
        } 
      }
      else if ((byte=='>') && (receiveBufferIndex==1)) {
        receiveBufferIndex = 0;
        resultATcommand (commandIdentifier, AT_RESULT_SEND_READY);
      }
      else if ((byte==' ') && (receiveBufferIndex==1)) {
        receiveBufferIndex = 0;
      }
    }      
  }
  
  
    
  public:
  void handler () override {   // override CRemoteXYNet
       
    stream->handler ();
    
    if (!ipdSize) { 
      if (commandIdentifier) {
        if (millis() - commandTimeOut > commandDelay) {
          resultATcommand (commandIdentifier, AT_RESULT_TIMEOUT);
        }
      } 
      else if (findModuleTryCount) {      
        if (millis() - lastAnswerTime > REMOREXYNET_AT__COMMAND_TIMEOUT) { 
          findModuleTryCount--;
          if (findModuleTryCount == 0) {
#if defined(REMOTEXY__DEBUGLOG)
            RemoteXYDebugLog.write ("Мodule does not respond to AT commands");
#endif 
            findModuleTryCount = 10;
          }
          lastAnswerTime = millis ();
          sendATCommand (AT_ID_FINDMODULE, AT_MESSAGE_AT, NULL);       
        }      
      } 
      else {
        if (millis() - lastAnswerTime > REMOREXYNET_AT__TEST_TIMEOUT) { 
          haveEcho = 0;
          if (sendATCommand (AT_ID_TESTMODULE, AT_MESSAGE_AT, NULL)) {
            lastAnswerTime = millis();           
          }
        }
      }   
    }
  }  
  
  
  private:
  void clientClosed () {
    uint8_t id = getATParamInt(0);
    CRemoteXYNet_AT_Client * p = clients;
    while (p) {
      if ((p->id == id) && (p->connected)) {
        p->connected = 0;
        //return;
      }
      p = p->next;
    }
  }  
  
  
  public:
  CRemoteXYNet_AT_Client * client_connect (const char *host, uint16_t port) {
    char sport[6]; 
    char sid[2];    
    int8_t i;
    CRemoteXYNet_AT_Client * p;
    clientConnectingID = REMOREXYNET_AT__NO_CHANNEL;
    for (i=3; i>=0; i--) {
      p = clients;
      while (p) {
        if ((p->connected) && (p->id == i)) break;
        p=p->next;
      }
      if (!p) {
        clientConnectingID = i;
        break;
      }
    }
    if (clientConnectingID == REMOREXYNET_AT__NO_CHANNEL) return NULL;
    rxy_intToStr (port, sport);
    *sid=clientConnectingID+0x30;
    *(sid+1)=0;  
    setATTimeOut (REMOREXYNET_AT__CONNECT_TIMEOUT);
    uint8_t res = sendATCommandForResult (AT_MESSAGE_CIPSTART, sid, ",\"TCP\",\"", host,"\",", sport,NULL);
    i = clientConnectingID;
    clientConnectingID = REMOREXYNET_AT__NO_CHANNEL; 
    if (res != AT_RESULT_OK) return NULL;
    p = getUnusedClient ();
    p->id = i;
    p->connected = 1;  
    return p;
  }
  
  public:
  void client_stop (CRemoteXYNet_AT_Client * client) {
    if (client->connected) {  
      client->connected = 0;
      char s[2];
      *s=client->id+0x30;
      *(s+1) = 0;
      sendATCommandForResult (AT_MESSAGE_CIPCLOSE,s,NULL);
    }
  }
  
  public:
  void client_startWrite (CRemoteXYNet_AT_Client * client, uint16_t size) {
    if (client->connected) {  
      char s[8];
      sendByteSize = size;
      if (sendByteSize > sendByteMax) sendByteSize = sendByteMax;
      sendByteNext = size - sendByteSize;
      
      rxy_intToStr (sendByteSize, s+2);
      *s=client->id+0x30;
      *(s+1)=',';      
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.writeInputNewString ();
#endif  
      uint8_t res = sendATCommandForResult (AT_MESSAGE_CIPSEND,s,NULL);     
      if (res == AT_RESULT_OK) res = waitATResult ();
      if (res == AT_RESULT_SEND_READY) {
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.writeAdd (" *send ");
        RemoteXYDebugLog.writeAdd (sendByteSize);
        RemoteXYDebugLog.writeAdd (" bytes*");
#endif   
        return;
      }
      sendByteSize = 0;
    }
  }
  
  public:
  void client_write (CRemoteXYNet_AT_Client * client, uint8_t byte) {
    if (sendByteSize) {
      stream->write (byte); 
      sendByteSize--;
      if (sendByteSize == 0) {
        setATTimeOut (REMOREXYNET_AT__SEND_TIMEOUT);
        if (waitATResult () == AT_RESULT_SEND_OK) {
          if (sendByteNext) client_startWrite (client, sendByteNext);
        }
      }
    }
  }
  
  public:
  uint8_t server_begin (uint16_t port, CRemoteXYNet_AT_ClientAvalaibleListener * listener) {
    char sport[6];    
    rxy_intToStr (port, sport);
    if (sendATCommandForResult ("AT+CIPSERVER=1,", sport, NULL) != AT_RESULT_OK) return 0;  
    if (sendATCommandForResult ("AT+CIPSTO=30", NULL) != AT_RESULT_OK) return 0;
    clientAvalaibleListener = listener;  
    return 1;  
  }
  
  public:
  void server_stop () {
    sendATCommandForResult ("AT+CIPSERVER=0", NULL);
    clientAvalaibleListener = NULL;
  }
    
  public:
  void findModule () {  
    findModuleTryCount=10;
    lastAnswerTime = millis ();
  }
  
  
  protected:
  uint8_t strcmpReceiveBuffer (const char * temp) {
    uint8_t k = 0;
    char * str = receiveBuffer;
    while (*temp) {
      if (!*str) return 1;
      switch (*temp) {
        case '?':
          params[k]=str;
          paramsLength[k]=1;
          temp++; 
          str++;   
          k++; 
          break;
        case '*':
          params[k]=str;
          paramsLength[k]=0; 
          temp++;
          while (*str!=*temp) {
            if (!*str++) return 1; 
            paramsLength[k]++;
          }
          k++;
          break;
        default: 
          if (*(str++)!=*(temp++)) return 1;
          break;
      }
    }
    if (*str) return 1;
    return 0;
  }  
  
  protected:
  uint16_t getATParamInt (uint8_t k) {
    uint16_t res = 0;
    char * p=params[k];
    uint8_t i=paramsLength[k];
    while (i--) res = res*10+(*p++)-'0';
    return res;
  }  
  
  
  public:
  CRemoteXYNet_AT_Client * getUnusedClient () {
    CRemoteXYNet_AT_Client * p = clients;
    while (p) {
      if (p->connected == 0) return p;
      p = p->next;
    }
    p = new CRemoteXYNet_AT_Client ();
    p->next = clients;
    clients = p; 
    return p;  
  } 

};

class CRemoteXYClient_AT : public CRemoteXYClient, public CRemoteXYNet_AT_ReadByteListener {

  public:
  CRemoteXYNet_AT * net;
  CRemoteXYNet_AT_Client * client;

  public:
  CRemoteXYClient_AT (CRemoteXYNet_AT * _net) {
    net = _net;
    client = NULL;
  }
  

  public:
  uint8_t connect (const char *host, uint16_t port) override {
    client = net->client_connect (host, port);
    if (client) {
      client->setReadByteListener (this);
      return client->connected;
    }
    return 0;
  }
  
  public:
  uint8_t connected () override {
    if (client) return client->connected;
    return 0;
  }
  
  public:
  void stop () override {
    if (client) net->client_stop (client);
  }

  void startWrite (uint16_t size) override {
    if (client) net->client_startWrite (client, size);
  }   
  
  void write (uint8_t byte) override {
    if (client) net->client_write (client, byte);
  }   
  
  void readByte (uint8_t byte) override {
    notifyReadByteListener (byte);
  }
       
};
        

class CRemoteXYServer_AT : public CRemoteXYServer, public CRemoteXYNet_AT_ClientAvalaibleListener {  

  protected:
  CRemoteXYNet_AT * net;
  uint16_t port;
  
  public:
  CRemoteXYServer_AT (CRemoteXYNet_AT * _net, uint16_t _port) {
    net = _net;
    port = _port;
  } 

       
  uint8_t begin () override {
    return net->server_begin (port, this);
  }
  
  void stop () override {
    net->server_stop ();
  }
  
  void clientAvailable (CRemoteXYNet_AT_Client * atClient) override {
    CRemoteXYClient_AT * client = (CRemoteXYClient_AT*)getUnusedClient();
    if (client == NULL) {
      client = new CRemoteXYClient_AT (net);
      addClient (client);
    }
    client->client = atClient;    
    notifyClientAvailableListener (client);        
  }

}; 


#endif  // RemoteXYNet_AT_h
