#ifndef RemoteXYNet_Modem_h
#define RemoteXYNet_Modem_h

#include "RemoteXYModem.h"
#include "RemoteXYNet.h"



#define REMOTEXY_NET_MODEM_CLIENT_MAX_COUNT 4
#define REMOTEXY_NET_MODEM_SEND_BUFFER_SIZE 32

#define REMOTEXY_NET_MODEM_CLIENT_CONNECT_TIMEOUT 20000


class CRemoteXYNet_Modem_Proto : public CRemoteXYModem, public CRemoteXYNet {

  public:
  uint8_t maxClientCount;
  int8_t readByteClientId;
  int8_t connectingClientId;
  CRemoteXYServer * server;
  CRemoteXYClient * clientsById[REMOTEXY_NET_MODEM_CLIENT_MAX_COUNT];

  public:
  CRemoteXYNet_Modem_Proto (CRemoteXYStream * stream) : CRemoteXYModem (stream), CRemoteXYNet () {
    maxClientCount = REMOTEXY_NET_MODEM_CLIENT_MAX_COUNT;
    server = NULL; 
    readByteClientId = -1;
    for (uint8_t i = 0; i < REMOTEXY_NET_MODEM_CLIENT_MAX_COUNT; i++) {
      clientsById[i] = NULL;
    }
    connectingClientId = -1;
  }
  
  public:
  virtual uint8_t startServer (uint16_t port) = 0;
  virtual uint8_t stopServer () = 0;
  virtual uint8_t doConnectClient (uint8_t clientId, const char *host, uint16_t port) = 0; // return clientId or -1
  virtual uint8_t doDisconnectClient (uint8_t clientId) = 0;  
  virtual uint8_t sendBytes (uint8_t clientId, uint8_t * buf, uint16_t len) = 0;
  
  void handler () override { 
    modemHandler ();             
  }   
  
  public:
  void setMaxClientCount (uint8_t cnt) {
    if (cnt <= REMOTEXY_NET_MODEM_CLIENT_MAX_COUNT) maxClientCount = cnt;  
  }    
  
  CRemoteXYClient * getConnectedClient (int8_t clientId) {
    if (clientId < maxClientCount) return clientsById[clientId];
    return NULL;
  }
       
  int8_t getUnusedClientId () {    
    for (int8_t id = 0; id < maxClientCount; id++) { 
      if (clientsById[id] == NULL) return id;
    }
    return -1;
  }
  
  int8_t getClientId (CRemoteXYClient * client) {
    for (int8_t id = 0; id < maxClientCount; id++) { 
      if (clientsById[id] == client) return id;
    }  
    return -1;
  } 
  
  void markClientConnected (CRemoteXYClient * client, int8_t clientId) {
    if (clientId < maxClientCount) clientsById[clientId] = client;
  }  
   
  void markClientDisconnected (int8_t clientId) {
    if (clientId < maxClientCount) clientsById[clientId] = NULL;
  }  
  
};


class CRemoteXYClient_Modem : public CRemoteXYClient {
  private:
  CRemoteXYNet_Modem_Proto * modem;
  uint8_t sendBuffer[REMOTEXY_NET_MODEM_SEND_BUFFER_SIZE];
  uint16_t sendBufferCount;  
  uint8_t needFlush;
   
  public:
  CRemoteXYClient_Modem (CRemoteXYNet_Modem_Proto * _modem) : CRemoteXYClient () {
    modem = _modem; 
    clearBuffers ();  
  } 
  
  public:
  void clearBuffers () {
    sendBufferCount = 0;
    needFlush = 0;
  }
  
  public:
  uint8_t connect (const char *host, uint16_t port) override {
    stop ();  
    int8_t clientId = modem->getUnusedClientId ();
    if (clientId >= 0) {
      modem->connectingClientId = clientId;
      uint8_t res = modem->doConnectClient (clientId, host, port);
      modem->connectingClientId = -1;
      if (res) {
        modem->markClientConnected (this, clientId);
        clearBuffers ();
        return 1;
      }
    }
    return 0;
  };   
  
  public:
  uint8_t connected () override {
    return modem->getClientId (this) >= 0;
  };  
  
  public:
  void stop () override {
    int8_t clientId = modem->getClientId (this);
    if (clientId >= 0) {
      modem->doDisconnectClient (clientId);
      modem->markClientDisconnected (clientId); 
    }
  };  
  
  
  public:
  void write (uint8_t b) override {
    if (needFlush) flush ();
    if (needFlush) {
      if (sendBufferCount >= REMOTEXY_NET_MODEM_SEND_BUFFER_SIZE) return;
    }
    int8_t clientId = modem->getClientId (this);
    if (clientId >= 0) {
      sendBuffer[sendBufferCount++] = b;      
      if (sendBufferCount >= REMOTEXY_NET_MODEM_SEND_BUFFER_SIZE) {
        flush ();
      } 
    }
  } 
  
  public:
  void flush () override {
    int8_t clientId = modem->getClientId (this);
    if (clientId >= 0) {
      if (sendBufferCount > 0) {
        if (modem->sendBytes (clientId, sendBuffer, sendBufferCount) == 0) {
          needFlush = 1;
          return;
        }
        sendBufferCount=0;
      }  
      needFlush = 0;
    }
  }
  
  void handler () override {
    if (needFlush) flush ();
  }
  
};

class CRemoteXYServer_Modem : public CRemoteXYServer {

  private:
  uint16_t port;
  
  public:
  CRemoteXYServer_Modem (CRemoteXYNet_Modem_Proto * _modem, uint16_t _port) : CRemoteXYServer (_modem) {
    port = _port;
  }

  uint8_t begin () override {
    return ((CRemoteXYNet_Modem_Proto*)net)->startServer (port);
  } 
  
  void stop () override {
    ((CRemoteXYNet_Modem_Proto*)net)->stopServer ();
  }
      
};



class CRemoteXYNet_Modem : public CRemoteXYNet_Modem_Proto {

  
  public:
  CRemoteXYNet_Modem (CRemoteXYStream * stream) : CRemoteXYNet_Modem_Proto (stream) {
  }
      
  void clientConnected (uint8_t clientId) {
    if (clientId != connectingClientId) {
      if (server) {
        CRemoteXYClient_Modem *client = (CRemoteXYClient_Modem*)(server->getUnconnectedClient ());
        if (client) {
          markClientConnected (client, clientId);
          client->clearBuffers ();
          server->notifyClientAvailableListener (client);
        }  
      }  
    }
  }   
  
  void setReadByteClientId (uint8_t clientId, uint16_t len) {
    CRemoteXYClient * client = getConnectedClient(clientId);
    if (!client) clientConnected (clientId);
    readByteClientId = clientId;
    setReadByteMode (len);
  }    
  
  void onReceiveByte (uint8_t byte) override {
    if (readByteClientId >= 0) {
      CRemoteXYClient * client = getConnectedClient(readByteClientId);
      if (client) {
        client->notifyReadByteListener (byte);
      }    
    }
  }  
      
  public:  
  CRemoteXYServer * createServer (uint16_t _port) override {
    if (server) return NULL;
    server = new CRemoteXYServer_Modem (this, _port);
    return server;
  }
  
  public:
  CRemoteXYClient * newClient () override {
    return new CRemoteXYClient_Modem (this);
  }
    
};
  


#endif //RemoteXYNet_Modem_h