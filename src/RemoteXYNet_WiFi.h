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

#if defined (WiFiS3_h) 
  // The server.available () function can return the same client 
  // even if it has been connected for a long time.
  // Need to cut off clients who are already working 
#define REMOREXYNET_WIFI__ADDITIONAL_CLIENT_CONTROL
#endif

class CRemoteXYClient_WiFi : public CRemoteXYClient {
  public:
  WiFiClient client;
  
  uint8_t sendBuffer[REMOREXYNET_WIFI__SEND_BUFFER_SIZE];
  uint16_t sendBufferCount; 
  uint16_t sendBytesAvailable;  
  
#if defined (REMOREXYNET_WIFI__ADDITIONAL_CLIENT_CONTROL) 
  uint8_t connectedFromServer;
#endif

  public:
  uint8_t connect (const char *host, uint16_t port) override {
    return client.connect(host, port);
  }; 
  
  public:
  uint8_t connected () override {
    return client.connected();
  };

  public:
  void stop () override {
    client.stop ();
  };
  
  public:
  void handler () override {   
    while (client.available ()) notifyReadByteListener (client.read ());
  }
  
  
  public:
  void startWrite (uint16_t len) override {
    sendBytesAvailable = len;
    sendBufferCount = 0;
  }  
  
  public:
  void write (uint8_t b) override {
    sendBuffer[sendBufferCount++] = b;
    sendBytesAvailable--;       
    if ((sendBufferCount == REMOREXYNET_WIFI__SEND_BUFFER_SIZE) || (sendBytesAvailable==0)) {
      client.write (sendBuffer, sendBufferCount);
      sendBufferCount=0;
    } 
  } 
  
};

class CRemoteXYServer_WiFi : public CRemoteXYServer {
  private:
  WiFiServer * server;

  public: 
  CRemoteXYServer_WiFi (uint16_t _port)  {
    server = new WiFiServer (_port); 
  }
  
  
  public:  
  uint8_t begin () override {

#if defined (REMOREXYNET_WIFI__ADDITIONAL_CLIENT_CONTROL)
    CRemoteXYClient_WiFi * client = (CRemoteXYClient_WiFi*)clients;
    while (client) {
      client->connectedFromServer = 0;
      client = (CRemoteXYClient_WiFi*)client->next;
    }
#endif // REMOREXYNET_WIFI__ADDITIONAL_CLIENT_CONTROL


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
      
#if defined (REMOREXYNET_WIFI__ADDITIONAL_CLIENT_CONTROL)
    client = (CRemoteXYClient_WiFi*)clients;
    while (client) {
      if (!client->client.connected ()) {
        client->connectedFromServer = 0;
      }
      client = (CRemoteXYClient_WiFi*)client->next;
    }
#endif // REMOREXYNET_WIFI__ADDITIONAL_CLIENT_CONTROL


#if defined (REMOREXYNET_WIFI__ESP32v3)
    WiFiClient cl = server->accept ();
#else      
    WiFiClient cl = server->available ();
#endif

    if (cl) {
    
      if (cl.connected ()) {
      
#if defined (ESP8266) 
        cl.disableKeepAlive ();     // remove memory leak
#endif // ESP8266
               
#if defined (REMOREXYNET_WIFI__ADDITIONAL_CLIENT_CONTROL) 
        client = (CRemoteXYClient_WiFi*)clients;
        while (client) {
          if (client->connectedFromServer != 0) {
            if (client->client == cl) return; // already use it
          }
          client = (CRemoteXYClient_WiFi*)client->next;
        }
#endif // REMOREXYNET_WIFI__ADDITIONAL_CLIENT_CONTROL

        client = (CRemoteXYClient_WiFi*)getUnusedClient ();
        if (client == NULL) {
          client = new CRemoteXYClient_WiFi ();
          addClient (client);
        }
        client->client = cl;
        
#if defined (REMOREXYNET_WIFI__ADDITIONAL_CLIENT_CONTROL) 
        client->connectedFromServer = 1;
#endif // REMOREXYNET_WIFI__ADDITIONAL_CLIENT_CONTROL

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
    
    //if (data->realTime != NULL) data->realTime->setNet (this);
    
#if defined(REMOTEXY__DEBUGLOG)
    uint8_t prev_wifiStatus = wifiStatus;
#endif      

    wifiStatus = WiFi.status();
        
    if (wifiStatus == WL_CONNECTED) {
      timeOut = millis ();  
#if defined(REMOTEXY__DEBUGLOG)
      if (prev_wifiStatus != WL_CONNECTED) {
        RemoteXYDebugLog.write ("WiFi connected");
        RemoteXYDebugLog.write ("IP: ");
        RemoteXYDebugLog.serial->print (WiFi.localIP());    
      }  
#endif      
    }
    else {  // != WL_CONNECTED 
    
#if defined(REMOTEXY__DEBUGLOG)
      if (prev_wifiStatus == WL_CONNECTED) { 
        RemoteXYDebugLog.write ("WiFi disconnected");
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
      RemoteXYDebugLog.write("WiFi module was not found");    
#endif 
      return;
    }
    
#endif // ESP
    
        
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Сonnecting to WiFi: ");
    RemoteXYDebugLog.writeAdd(wifiSsid);
    RemoteXYDebugLog.writeAdd(" ...");   
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
    return new CRemoteXYServer_WiFi (_port);
  }
  

  CRemoteXYClient * newClient () override {
    return new CRemoteXYClient_WiFi ();
  }
  
  /*
  CRemoteXYHttpRequest_Proto * createHttpRequest () override { 
    return new CRemoteXYHttpRequest (this);
  }         
  */
                  
};


#define CRemoteXYComm_WiFiPoint CRemoteXYNet_WiFiPoint
class CRemoteXYNet_WiFiPoint : public CRemoteXYNet {
  
  uint8_t state;
  
  public:
  CRemoteXYNet_WiFiPoint (const char * _wifiSsid, const char * _wifiPassword) : CRemoteXYNet () {

    
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Creating WiFi point: ");
    RemoteXYDebugLog.writeAdd(_wifiSsid);
    RemoteXYDebugLog.writeAdd(" ...");
#endif

#if defined (ESP8266) || defined (ESP32) 
// ESP8266 and ESP32
    
    WiFi.mode(WIFI_AP);
    WiFi.softAP(_wifiSsid, _wifiPassword);
    state = 1;
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("WiFi point created");
    RemoteXYDebugLog.write ("IP: ");
    RemoteXYDebugLog.serial->print (WiFi.softAPIP());    
#endif

    
#elif defined (WiFiNINA_h) || defined (WiFiS3_h) || defined (ARDUINO_ARCH_MBED) 
// WiFiNINA.h
// WiFiS3.h
// MBED platform

    state = 0;
    if (WiFi.status() == WL_NO_SHIELD) {    
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write("WiFi module was not found");    
#endif 
      return;
    }
    if (WiFi.beginAP (_wifiSsid, _wifiPassword) != WL_AP_LISTENING) {
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write("WiFi point was not created");    
#endif 
      return;    
    }
    state = 1;
    
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("WiFi point created");    
    RemoteXYDebugLog.write ("IP: ");
    RemoteXYDebugLog.serial->print (WiFi.localIP());  
#endif  

#else  
// other boards not support AP mode

    state = 0;
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("WiFi module does not support AP mode");    
#endif 
  
#endif // ESP

  }
              
  public:                   
  uint8_t configured () override {
    return state;
  } 
  
    
  public:  
  CRemoteXYServer * createServer (uint16_t _port) override {   
    return new CRemoteXYServer_WiFi (_port); 
  }
  

  CRemoteXYClient * newClient () override {
    return new CRemoteXYClient_WiFi ();
  }


};


#endif // WiFi_h

#endif //RemoteXYNet_WiFi_h