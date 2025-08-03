#ifndef RemoteXYType_Print_h
#define RemoteXYType_Print_h

#include "RemoteXYFunc.h"
#include "RemoteXYType_HeapPrintable.h"



#pragma pack(push, 1)

struct RemoteXYType_Print_Color {
  uint8_t mask; // 0 if default color 
  uint8_t r;  
  uint8_t g;  
  uint8_t b;  
};
#pragma pack(pop)
                  
class CRemoteXYTypeInner_Print : public CRemoteXYTypeInner_HeapPrintable {
  
  RemoteXYType_Print_Color color;
  
  
  public:
  uint8_t* init (uint8_t *conf) override  {         
    color.mask = 0;  
    color.r = color.g = color.b = 0;      
    return CRemoteXYTypeInner_HeapPrintable::init (conf);
  };

            
                  
  public:   
  void addBufferToHeap () override {
    uint16_t cs = 1;
    if (color.mask != 0) cs = sizeof (RemoteXYType_Print_Color);
    if (addToHeap ((uint8_t*)&color, cs, printBffer, printBufferLength)) {    
      printBufferLength = 0;  
    }
  }
   
  
  void setColor (uint8_t r, uint8_t g, uint8_t b) {
    send ();
    color.r = r;
    color.g = g;
    color.b = b; 
    color.mask = 1;    
  }     
         
  void setColor (uint32_t _color) {
    send ();
    color.r = _color >> 16;
    color.g = _color >> 8;
    color.b = _color; 
    color.mask = 1;    
  }  
  
  void setDefaultColor () {
    send ();
    color.mask = 0;
  }  
  
  void handler () override {      
    send ();
  };   
   
};
              
#define RemoteXYType_Print_inner ((CRemoteXYTypeInner_Print*)inner)
 
#pragma pack(push, 1) 
class RemoteXYType_Print : public CRemoteXYType_HeapPrintable {

  public:
  RemoteXYType_Print () {
    inner = new CRemoteXYTypeInner_Print ();
  } 
  
  void setColor (uint8_t r, uint8_t g, uint8_t b) {
    RemoteXYType_Print_inner->setColor (r, g, b); 
  }     
         
  void setColor (uint32_t _color) {
    RemoteXYType_Print_inner->setColor (_color); 
  }  
  
  void setDefaultColor () {
    RemoteXYType_Print_inner->setDefaultColor (); 
  }      
             
};  
      
#pragma pack(pop)

#endif // RemoteXYType_Print_h