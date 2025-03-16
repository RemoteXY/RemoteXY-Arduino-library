#ifndef RemoteXYNet_h
#define RemoteXYNet_h  

#include <inttypes.h> 
#include "RemoteXYDebugLog.h"
#include "RemoteXYStream.h"

class CRemoteXYData;


#define UNUSED(x) (void)(x)


class CRemoteXYClient: public CRemoteXYStream {
  public:
  CRemoteXYClient * next;
  
  public:
  CRemoteXYClient (): CRemoteXYStream() {}
    
  public: 
  virtual uint8_t connect (const char *host, uint16_t port) {UNUSED (host); UNUSED (port); return 0;};  
  virtual void stop () {};  
  virtual uint8_t connected () {return 1;};  
  virtual uint8_t equal (CRemoteXYClient * cl) {UNUSED (cl); return 0;} 
};

class CRemoteXYClientAvailableListener {
  public:
  virtual void clientAvailable (CRemoteXYClient * client) = 0;
};

class CRemoteXYServer {

  public:
  CRemoteXYClient * clients; 
  CRemoteXYClientAvailableListener * clientAvailableListener; 
  
  public:      
  CRemoteXYServer () {
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
  CRemoteXYClient * getUnusedClient () {
    CRemoteXYClient * pc = clients;
    while (pc) {
      if (!pc->connected ()) break;
      pc = pc->next;
    }   
    return pc;
  }
  
  public:
  void addClient (CRemoteXYClient * client) {
    if (client) {
      client->next = clients;
      clients = client;
    }  
  }
   
  public:     
  virtual uint8_t begin () {return 0;} 
  virtual void handler () {}; 
  virtual void stop () {}; 

};

class CRemoteXYHttpRequest;


class CRemoteXYNet { 
  public:
  CRemoteXYNet * next;
  CRemoteXYHttpRequest * httpRequest;

  public:
  virtual void handler () {}; 
  virtual uint8_t configured () {return 1;};                 
  virtual CRemoteXYServer * createServer (uint16_t _port) {UNUSED (_port); return NULL;}  
  virtual CRemoteXYClient * newClient () {return NULL;}
  virtual uint8_t hasInternet () {return 0;}
 
};



#endif //RemoteXYNet_h