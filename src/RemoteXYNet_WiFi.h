#ifndef RemoteXYNet_WiFi_h
#define RemoteXYNet_WiFi_h

#include "RemoteXYFunc.h" 
#include "RemoteXYDebugLog.h"
#include "RemoteXYNet.h"
 
#if defined (WIFI_AP) && defined (WIFI_STA) && defined (ESP32) && ESP_ARDUINO_VERSION_MAJOR >= 3
#define REMOREXYNET_WIFI__ESP32v3
#endif

 // only WiFi.h library
#if defined (WiFi_h) || defined (WiFiNINA_h) || defined (WiFiS3_h) || defined (REMOREXYNET_WIFI__ESP32v3) 

#if defined (ESP8266) || defined (ESP32) || defined (WiFiS3_h)
#define REMOREXYNET_WIFI__SEND_BUFFER_SIZE 256
#else  // arduino shield   
#define REMOREXYNET_WIFI__SEND_BUFFER_SIZE 64
#endif

#define REMOREXYNET_WIFI__RECONNECT_TIMEOUT 20000  // reconnect in 20 sec



class CRemoteXYClient_WiFi : public CRemoteXYClient {
  public:
  WiFiClient client;
  uint8_t clientConnected;
  
  uint8_t sendBuffer[REMOREXYNET_WIFI__SEND_BUFFER_SIZE];
  uint16_t sendBufferCount;  

  public:
  CRemoteXYClient_WiFi () : CRemoteXYClient () {
    sendBufferCount = 0;
    clientConnected = 0;
  }
  
  void beginFromServer (WiFiClient _client) {
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
      if (sendBufferCount == REMOREXYNET_WIFI__SEND_BUFFER_SIZE) {
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

class CRemoteXYServer_WiFi : public CRemoteXYServer {
  private:
  WiFiServer * server;
  CRemoteXYNet * net;
  

  public: 
  CRemoteXYServer_WiFi (CRemoteXYNet* _net, uint16_t _port): CRemoteXYServer (_net) {
    server = new WiFiServer (_port); 
  }
  
  
  public:  
  uint8_t begin () override {
    server->begin (); 
    return 1;   
  }
  
 
  void stop () override {
#if defined (ESP8266) || defined (ESP32)  
    server->stop ();  
#endif
  } 
 

  public:
  void handler () override {
    CRemoteXYClient_WiFi * client;
    
       
#if defined (ESP8266) || defined (ESP32)  
    if (!server->hasClient()) return; 
#endif        
      

#if defined (REMOREXYNET_WIFI__ESP32v3) || defined (ESP8266)
    WiFiClient cl = server->accept ();
#else      
    WiFiClient cl = server->available ();
#endif

    if (cl) {    
      if (cl.connected ()) {
      
#if defined (WiFiS3_h) 
// The server.available () function can return the same client 
// even if it has been connected for a long time.
// Need to cut off clients who are already working 

        client = (CRemoteXYClient_WiFi*)clients;
        while (client) {
          if ((client->client == cl) && (client->connected())) return;
          client = (CRemoteXYClient_WiFi*)client->next;
        } 
        
#elif defined (WiFiNINA_h)    
 
        client = (CRemoteXYClient_WiFi*)clients;
        while (client) {
          if ((client->client.remotePort() == cl.remotePort()) && (client->connected())) return;
          client = (CRemoteXYClient_WiFi*)client->next;
        }  
                
#endif 
        
#if defined (ESP8266) 
        cl.disableKeepAlive ();     // remove memory leak
#endif // ESP8266

        client = (CRemoteXYClient_WiFi*)getUnconnectedClient ();
        client->beginFromServer (cl);    
        notifyClientAvailableListener (client);        
      }
    }
  } 
  
};

#define CRemoteXYComm_WiFi CRemoteXYNet_WiFi
class CRemoteXYNet_WiFi : public CRemoteXYNet {

  private:
  char * wifiSsid;
  char * wifiPassword;
  uint8_t wifiStatus;
  uint32_t timeOut;  // for reconnect
  
  
  public:
  CRemoteXYNet_WiFi (const char * _wifiSsid, const char * _wifiPassword) : CRemoteXYNet () {
    wifiSsid = (char *)_wifiSsid;
    wifiPassword = (char *)_wifiPassword;
    
    timeOut = millis ();     
    wifiStatus = WiFi.status();
    connectToWiFi ();    
    
  }
  
  
  void handler () override {            
    
    
#if defined(REMOTEXY__DEBUGLOG)
    uint8_t prev_wifiStatus = wifiStatus;
#endif      

    wifiStatus = WiFi.status();
        
    if (wifiStatus == WL_CONNECTED) {
      timeOut = millis ();  
#if defined(REMOTEXY__DEBUGLOG)
      if (prev_wifiStatus != WL_CONNECTED) {
        RemoteXYDebugLog.write (F("WiFi connected"));
        RemoteXYDebugLog.write (F("IP: "));
        RemoteXYDebugLog.serial->print (WiFi.localIP());    
      }  
#endif      
    }
    else {  // != WL_CONNECTED 
    
#if defined(REMOTEXY__DEBUGLOG)
      if (prev_wifiStatus == WL_CONNECTED) { 
        RemoteXYDebugLog.write (F("WiFi disconnected"));
      }
#endif

      if (millis() - timeOut > REMOREXYNET_WIFI__RECONNECT_TIMEOUT) {
        timeOut = millis ();  
        connectToWiFi ();        
      }
    }
  } 
  
  private:
  void connectToWiFi () {
  
#if defined (ESP8266) || defined (ESP32)

    WiFi.disconnect();
    WiFi.softAPdisconnect(true);    
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);    
    
#else // NOT ESP    
       
    if (wifiStatus == WL_NO_SHIELD) {    
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write(F("WiFi module was not found"));    
#endif 
      return;
    }
    
#endif // ESP
    
        
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("Сonnecting to WiFi: "));
    RemoteXYDebugLog.writeAdd(wifiSsid);
    RemoteXYDebugLog.writeAdd(F(" ..."));   
#endif    

    WiFi.begin(wifiSsid, wifiPassword);
  
  }
  
         
  public:                   
  uint8_t configured () override {
    if (wifiStatus == WL_CONNECTED) return 1;
    return 0;
  }                
  
  public:  
  CRemoteXYServer * createServer (uint16_t _port) override {
    return new CRemoteXYServer_WiFi (this, _port);
  }
  

  
  uint8_t hasInternet () override {
    return 1;
  }
      
  public:
  CRemoteXYClient * newClient () override {
    return new CRemoteXYClient_WiFi ();
  }            
};


#define CRemoteXYComm_WiFiPoint CRemoteXYNet_WiFiPoint
class CRemoteXYNet_WiFiPoint : public CRemoteXYNet {
  
  uint8_t state;
  
  public:
  CRemoteXYNet_WiFiPoint (const char * _wifiSsid, const char * _wifiPassword) : CRemoteXYNet () {

    
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("Creating WiFi point: "));
    RemoteXYDebugLog.writeAdd(_wifiSsid);
    RemoteXYDebugLog.writeAdd(F(" ..."));
#endif

#if defined (ESP8266) || defined (ESP32) 
// ESP8266 and ESP32
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP(_wifiSsid, _wifiPassword);
    state = 1;
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("WiFi point created"));
    RemoteXYDebugLog.write (F("IP: "));
    RemoteXYDebugLog.serial->print (WiFi.softAPIP());    
#endif

    
#elif defined (WiFiNINA_h) || defined (WiFiS3_h) || defined (ARDUINO_ARCH_MBED) 
// WiFiNINA.h
// WiFiS3.h
// MBED platform

    state = 0;
    if (WiFi.status() == WL_NO_SHIELD) {    
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write(F("WiFi module was not found"));    
#endif 
      return;
    }
    if (WiFi.beginAP (_wifiSsid, _wifiPassword) != WL_AP_LISTENING) {
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write(F("WiFi point was not created"));    
#endif 
      return;    
    }
    state = 1;
    
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("WiFi point created"));    
    RemoteXYDebugLog.write (F("IP: "));
    RemoteXYDebugLog.serial->print (WiFi.localIP());  
#endif  

#else  
// other boards not support AP mode

    state = 0;
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("WiFi module does not support AP mode"));    
#endif 
  
#endif // ESP

  }
              
  public:                   
  uint8_t configured () override {
    return state;
  } 
  
    
  public:  
  CRemoteXYServer * createServer (uint16_t _port) override {   
    return new CRemoteXYServer_WiFi (this, _port); 
  }
  
  public:
  CRemoteXYClient * newClient () override {
    return new CRemoteXYClient_WiFi ();
  }


};


#endif // WiFi_h

#endif //RemoteXYNet_WiFi_h