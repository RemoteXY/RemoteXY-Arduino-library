#ifndef RemoteXYType_h
#define RemoteXYType_h



class CRemoteXYGuiData;
class CRemoteXYPackage;
class CRemoteXYWire;



class CRemoteXYTypeInner {

  public:
  CRemoteXYGuiData * guiData;

  public:     

  void setGuiData (CRemoteXYGuiData * _guiData) {
    guiData = _guiData;
  }
  
  virtual uint8_t* init (uint8_t *conf) = 0;
  virtual uint8_t getDescriptorLength () = 0;
  virtual void sendDescriptorBytes (CRemoteXYWire * wire) = 0;
  virtual uint8_t handlePackage (CRemoteXYPackage * package, CRemoteXYWire * wire) = 0;  
  virtual void handler () {};

  
};


// These structures and classes participate in data 
// exchange or can be included in the RemoteXY structure. 
// They may not be address aligned on a 64-bit processor.

#pragma pack(push, 1)  

class CRemoteXYType {
  public:
  CRemoteXYTypeInner * inner;
  
  virtual uint16_t getTypeSize () {
    return sizeof (CRemoteXYType);
  }
    
};

#pragma pack(pop)


#endif // RemoteXYType_h