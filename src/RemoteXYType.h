#ifndef RemoteXYType_h
#define RemoteXYType_h



class CRemoteXYGuiData;
class CRemoteXYPackage;
class CRemoteXYWire;

class CRemoteXYType {

  public:
  CRemoteXYGuiData * guiData;

  public:     

  void setGuiData (CRemoteXYGuiData * _guiData) {
    guiData = _guiData;
  }
  
  virtual uint16_t sizeOf () = 0; 
  virtual uint8_t* init (uint8_t *conf) = 0;
  virtual uint16_t getDescriptorLength () = 0;
  virtual void getDescriptor (uint8_t * buf) = 0;
  virtual uint8_t receivePackage (CRemoteXYPackage * package, CRemoteXYWire * wire) = 0;  
  virtual void handler () {};

  
};



#endif // RemoteXYType_h