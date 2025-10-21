#ifndef RemoteXYNet_Ethernet_h
#define RemoteXYNet_Ethernet_h

 
#if defined (ethernet_h_) 

#include "RemoteXYDebugLog.h"
#include "RemoteXYNet.h"
#include "RemoteXYFunc.h"


#define REMOREXYNET_ETHERNET__SEND_BUFFER_SIZE 32

#if defined (ESP32) && ESP_ARDUINO_VERSION_MAJOR <= 2
#define REMOTEXYNET_ETHERNET__ESP32v2
#endif


class CRemoteXYClient_Ethernet : public CRemoteXYClient {
  public:
  EthernetClient client;
  uint8_t clientConnected;
  
  uint8_t sendBuffer[REMOREXYNET_ETHERNET__SEND_BUFFER_SIZE];
  uint16_t sendBufferCount; 

  public:
  CRemoteXYClient_Ethernet () : CRemoteXYClient() {
    sendBufferCount = 0;
    clientConnected = 0;
  }
  
  
  public:
  void begin (EthernetClient _client) {   
    client = _client;  
    sendBufferCount = 0;
    clientConnected = 1;
  }

  public:
  uint8_t connect (const char *host, uint16_t port) override {
    sendBufferCount = 0;
    clientConnected = client.connect(host, port);
    return clientConnected;
  }; 
  
  public:
  uint8_t connected () override {
    if (!client.connected()) clientConnected = 0;   
    return clientConnected;
  };

  public:
  void stop () override {
    client.stop ();
    clientConnected = 0;
  };
  
  public:
  void handler () override { 
    if (connected ()) {    
      while (client.available ()) notifyReadByteListener (client.read ());
    }
  }
         

  
  public:
  void write (uint8_t b) override {
    if (clientConnected) {
      sendBuffer[sendBufferCount++] = b;   
      if (sendBufferCount == REMOREXYNET_ETHERNET__SEND_BUFFER_SIZE) {  
        flush (); 
      } 
    }
  } 
  
  void flush () override {
    if (sendBufferCount > 0) {
      if (clientConnected) {
        client.write (sendBuffer, sendBufferCount);
      }
      sendBufferCount=0;    
    }
  }
     
};
    
     

class CRemoteXYServer_Ethernet : public CRemoteXYServer {
  private:
  CRemoteXYNet * net;
  EthernetServer * server;

  public: 
  CRemoteXYServer_Ethernet (CRemoteXYNet * _net, uint16_t _port): CRemoteXYServer (_net)  {
    server = new EthernetServer (_port); 
  }
  
  
  public:  
  uint8_t begin () override {
#ifndef REMOTEXYNET_ETHERNET__ESP32v2
    server->begin (); 
#endif
    return 1;   
  }
  
  void stop () override {
  }
         

  void handler () override {  
    EthernetClient cl = server->available (); 
    if (cl) {
      if (cl.connected ()) {  
        
        CRemoteXYClient_Ethernet * client = (CRemoteXYClient_Ethernet*)clients;
        while (client) {
          if ((client->client == (const EthernetClient)cl) && (client->connected())) return;
          client = (CRemoteXYClient_Ethernet*)client->next;
        } 
      
        if (client == NULL) {
          client = (CRemoteXYClient_Ethernet*)getUnconnectedClient ();
          client->begin (cl);   
          notifyClientAvailableListener (client);        
        } 
      }
    }   
  } 
  
};

#define CRemoteXYComm_Ethernet CRemoteXYNet_Ethernet
class CRemoteXYNet_Ethernet : public CRemoteXYNet {

  private:
  uint8_t mac[6];
  enum {NoHardware, LinkDetecting, Work} state;
  uint32_t timeout;
  
  public:
  CRemoteXYNet_Ethernet () : CRemoteXYNet () {   
     
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("Ethernet begin ..."));
#endif 

    Ethernet.begin(mac, 1000);
    
    if (Ethernet.hardwareStatus() == EthernetNoHardware) { 
      state = NoHardware;   
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write(F("Ethernet module was not found"));    
#endif 
    }
    else {
      state = LinkDetecting;         
#if defined(REMOTEXY__DEBUGLOG)
      if (!linkON ()) { 
        RemoteXYDebugLog.write(F("Ethernet link OFF"));
      }
#endif 
    }  
    timeout = millis ();  
  }
    
  public:
  CRemoteXYNet_Ethernet (const char * macAddress) : CRemoteXYNet () {   
    rxy_strParseMacAddr (macAddress, mac);  
     
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("Ethernet begin ..."));
#endif 

    Ethernet.begin(mac, 1000);
    
    if (Ethernet.hardwareStatus() == EthernetNoHardware) { 
      state = NoHardware;   
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write(F("Ethernet module was not found"));    
#endif 
    }
    else {
      state = LinkDetecting;         
#if defined(REMOTEXY__DEBUGLOG)
      if (!linkON ()) { 
        RemoteXYDebugLog.write(F("Ethernet link OFF"));
      }
#endif 
    }  
    timeout = millis ();  
  }
  

  private:
  uint8_t linkON () {
    if (uint32_t(Ethernet.localIP()) != 0) return 1;
    /*
    if (Ethernet.hardwareStatus () == EthernetW5100) {
      if (uint32_t(Ethernet.localIP()) != 0) return 1;
    }
    else {
      if (Ethernet.linkStatus () == LinkON) return 1;
    }
    */
    return 0;
  }
  
  
  public:
  void handler () override {          
    
    if (state == NoHardware) return;
    
    if (linkON ()) {
      if (state == LinkDetecting) {
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write (F("Ethernet link ON"));
        RemoteXYDebugLog.write(F("IP: "));
        RemoteXYDebugLog.serial->print(Ethernet.localIP());
#endif  
        state = Work;
      }
      timeout = millis (); 
    }
    else {  // LinkOFF
      if (state == Work) {
        state = LinkDetecting;
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write (F("Ethernet link OFF"));
#endif  
      }
      else {
        if (millis () - timeout > 5000) {
          Ethernet.begin(mac, 100);
          timeout = millis ();  
        }
      }
    }
  } 
  
         
  public:                   
  uint8_t configured () override {
    if (state == Work) return 1;
    return 0;
  }                
  
  public:  
  CRemoteXYServer * createServer (uint16_t _port) override {
    return new CRemoteXYServer_Ethernet (this, _port);
  }
  
  
  public:  
  uint8_t hasInternet () override {
    return 1;
  }
  
  public:
  CRemoteXYClient * newClient () override {
    return new CRemoteXYClient_Ethernet ();
  }

};


#endif // ethernet_h_

#endif //RemoteXYNet_Ethernet_h