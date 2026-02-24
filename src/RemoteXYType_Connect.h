#ifndef RemoteXYType_Connect_h
#define RemoteXYType_Connect_h

#include "RemoteXYType.h"
#include "RemoteXYThread.h"


#define UNUSED(x) (void)(x)

class CRemoteXYTypeInner_Connect : public CRemoteXYTypeInner {

  private:
  char * connectDesc;

  public:
  uint8_t* init (uint8_t *conf) override  {
    connectDesc = NULL;
    return conf;
  };
  
  public:
  uint8_t getDescriptorLength () override {
    return 0;
  }
  
  public:
  void sendDescriptorBytes (CRemoteXYWire * wire) override {
    UNUSED (wire);
  }
  
  public:
  void reset () {
    if (connectDesc) free (connectDesc);
    connectDesc = NULL;
  }
  
  public:
  void connectCloud (const char * server, uint16_t port, const char * token) {
    reset ();
    connectDesc = (char*)malloc (rxy_strLength(server) + rxy_strLength(token) + 18);
    if (connectDesc) {
      char * p = connectDesc;
      p = rxy_strCopy (p, "c=c&h=");
      p = rxy_strCopy (p, server);
      p = rxy_strCopy (p, "&r=");
      p = rxy_intToStr (port, p);
      p = rxy_strCopy (p, "&t=");
      p = rxy_strCopy (p, token);
      CRemoteXYThread::notifyComplexVarNeedSend (guiData);
    }
  }
  
  
  public:
  uint8_t handlePackage (CRemoteXYPackage * package, CRemoteXYWire * wire) override {
    if (connectDesc) {
      uint16_t len = rxy_strLength (connectDesc);
      wire->startPackage (REMOTEXY_PACKAGE_COMMAND_COMPLEXDATA, package->clientId, len); 
      wire->write ((uint8_t*)connectDesc, len); 
      reset ();
      return 1;
    }
    return 0;
  }     
  
};

#define RemoteXYType_Connect_inner ((CRemoteXYTypeInner_Connect*)inner)

#pragma pack(push, 1) 
class RemoteXYType_Connect: public CRemoteXYType {
  public:
  RemoteXYType_Connect () {
    inner = new CRemoteXYTypeInner_Connect ();
  }
  
  public:
  void reset () {
    RemoteXYType_Connect_inner->reset ();  
  }
  
  public:
  void connectCloud (const char * server, uint16_t port, const char * token) {
    RemoteXYType_Connect_inner->connectCloud (server, port, token);
  }  
  
  
  
};
#pragma pack(pop)


#endif // RemoteXYType_Connect_h