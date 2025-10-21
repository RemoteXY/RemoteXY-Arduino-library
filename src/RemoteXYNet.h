#ifndef RemoteXYNet_h
#define RemoteXYNet_h  

#include <inttypes.h> 
#include "RemoteXYDebugLog.h"
#include "RemoteXYStream.h"

class CRemoteXYData;


#define UNUSED(x) (void)(x)

class CRemoteXYClient;
class CRemoteXYServer;
class CRemoteXYHttpRequest;
class CRemoteXYServerRequest;


class CRemoteXYNet { 
  public:
  CRemoteXYNet * next;
  CRemoteXYHttpRequest * httpRequest;
  CRemoteXYServerRequest * serverRequest;
  
  public:
  CRemoteXYNet () {
    httpRequest = NULL;
    serverRequest = NULL;
    
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.init ();
#endif     
  }

  public:
  virtual void handler () {}; 
  virtual uint8_t configured () = 0;                 
  virtual CRemoteXYServer * createServer (uint16_t _port) = 0 ;  
  virtual uint8_t hasInternet () {return 0;}
  virtual CRemoteXYClient * newClient () = 0;
 
};


class CRemoteXYClient: public CRemoteXYStream {
  public:
  CRemoteXYClient * next;
  
  public:
  CRemoteXYClient (): CRemoteXYStream() {}
    
  public: 
  virtual uint8_t connect (const char *host, uint16_t port) = 0;  
  virtual void stop () = 0;  
  
  uint8_t connect (const __FlashStringHelper *fhost, uint16_t port) {
    char *host = (char*) malloc (rxy_strLength(fhost)+1);
    rxy_strCopy (host, fhost);
    uint8_t res = connect (host, port);
    free (host);
    return res;
  };  
  
};

class CRemoteXYClientAvailableListener {
  public:
  virtual void clientAvailable (CRemoteXYClient * client) = 0;
};

class CRemoteXYServer {

  public:
  CRemoteXYClientAvailableListener * clientAvailableListener; 
  CRemoteXYNet * net;
  CRemoteXYClient * clients; 
  
  public:      
  CRemoteXYServer (CRemoteXYNet * _net) {
    net = _net;
    clientAvailableListener = NULL; 
    clients = NULL;
  }
    
  public:
  void setClientAvailableListener (CRemoteXYClientAvailableListener * listener) {
    clientAvailableListener = listener;
  }    
         
  public:
  void notifyClientAvailableListener (CRemoteXYClient * client) {
    if (clientAvailableListener) clientAvailableListener->clientAvailable (client);
  }     
   
  public:     
  virtual uint8_t begin () = 0; 
  virtual void handler () {}; 
  virtual void stop () = 0; 
  
  public:
  CRemoteXYClient * getUnconnectedClient () {
    CRemoteXYClient * p = clients;
    while (p) {
      if (p->connected() == 0) break;
      p = p->next;
    }      
    if (!p) {
      p = net->newClient ();
      if (p) {
        p->next = clients;
        clients = p;
      }
    }
    return p;
  }  
  

};




#endif //RemoteXYNet_h