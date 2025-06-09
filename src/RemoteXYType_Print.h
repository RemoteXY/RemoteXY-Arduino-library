#ifndef RemoteXYType_Print_h
#define RemoteXYType_Print_h

#include "RemoteXYFunc.h"
#include "RemoteXYType_Heap.h"

#define REMOTEXY_TYPE_PRINT_BUFFER_LENGTH 24 // 32 - 4 -4


#pragma pack(push, 1)

struct RemoteXYType_Print_Color {
  uint8_t mask; // 0 if default color 
  uint8_t r;  
  uint8_t g;  
  uint8_t b;  
};
#pragma pack(pop)
                  
class CRemoteXYTypeInner_Print : public CRemoteXYTypeInner_Heap {
  
  uint8_t bffer[REMOTEXY_TYPE_PRINT_BUFFER_LENGTH];
  uint8_t bufferLength;
  RemoteXYType_Print_Color color;
  
  public:
  uint8_t* init (uint8_t *conf) override  {
    bufferLength = 0;           
    color.mask = 0;  
    color.r = color.g = color.b = 0;      
    return CRemoteXYTypeInner_Heap::init (conf);
  };

              
  private:
  void addToHeap_Color (const uint8_t * buf, uint16_t len) {
    if (color.mask == 0) addToHeap ((uint8_t*)&color, 1, bffer, bufferLength); 
    else addToHeap ((uint8_t*)&color, sizeof (RemoteXYType_Print_Color), bffer, bufferLength);     
  }   
                  
  private:   
  void addBufferToHeap () {
    addToHeap_Color (bffer, bufferLength);
    bufferLength = 0;  
  }
   
  
  public:
  void write (uint8_t b) {
    if (bufferLength >= REMOTEXY_TYPE_PRINT_BUFFER_LENGTH) {
      addBufferToHeap ();
    }
    bffer[bufferLength++] = b;   
  } 
  

  void write (const uint8_t *buf, uint16_t size) {
    // can't break bytes into different packets, you can break a UTF symbol
    if (size > 0) {
      if (bufferLength + size > REMOTEXY_TYPE_PRINT_BUFFER_LENGTH) {
        if (bufferLength > 0) addBufferToHeap ();
      }
      if (size > REMOTEXY_TYPE_PRINT_BUFFER_LENGTH) {
        addToHeap_Color (buf, size);
      }
      else {        
        while (size--) bffer[bufferLength++] = *buf++; 
      }
    }    
  }
    
  
  void handler () override {      
    if (bufferLength > 0) addBufferToHeap ();
  };      
  
  void setColor (uint8_t r, uint8_t g, uint8_t b) {
    handler ();
    color.r = r;
    color.g = g;
    color.b = b; 
    color.mask = 1;    
  }     
         
  void setColor (uint32_t _color) {
    handler ();
    color.r = _color >> 16;
    color.g = _color >> 8;
    color.b = _color; 
    color.mask = 1;    
  }  
  
  void setDefaultColor () {
    handler ();
    color.mask = 0;
  }  
   
};
              
#define RemoteXYType_Print_inner ((CRemoteXYTypeInner_Print*)inner) 
#pragma pack(push, 1) 
class RemoteXYType_Print : public CRemoteXYType {

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
  
  void write (uint8_t b) {
    RemoteXYType_Print_inner->write (b); 
  }
           
  void write (const uint8_t *buf, uint16_t size) {
    RemoteXYType_Print_inner->write (buf, size);   
  }        
  
   
  // Print  
    
  void print() {
  }
      
  void print (const __FlashStringHelper * str) {
    PGM_P p = reinterpret_cast<PGM_P>(str);
    uint8_t c;
    while (1) {
      c = pgm_read_byte(p++);
      if (c == 0) break;
      write(c);     
    }
  }
  
       
  void print(const char * str) {
    if (str != NULL) {
      write ((const uint8_t*)str, rxy_strLength (str));
    }
  }
  
  void print(String str) {
    print (str.c_str());
  }
  
  void print(char c) {
    write (c);
  }
  
  void print(unsigned char b, int base = 10) {
    return print((unsigned long) b, base);
  }
  
  void print(int n, int base = 10) {
    return print((long) n, base); 
  }
  
  void print(unsigned int n, int base = 10) {
    return print((unsigned long) n, base);
  }
  
  void print(long n, int base = 10) {
    if (base < 2) base = 10;
    uint8_t bufLen = rxy_uint32StrDigits (base) + 2;
    char buf[bufLen];
    char *p = buf;    
    if (n < 0) {
      *p++ = '-';
      n = -n;
    }
    rxy_intToStr (n, p, base);
    print (buf);
  }
  
  void print(unsigned long n, int base = 10) {
    if (base < 2) base = 10;
    uint8_t bufLen = rxy_uint32StrDigits (base) + 1;
    char buf[bufLen];
    rxy_intToStr (n, buf, base);
    print (buf);
  }
  
  void print(double number, int digits = 2) {
    uint8_t bufLen = 13+digits; 
    char buf[bufLen];
    char *p = buf;
    if (number < 0) {
      *p++ = '-';
      number = -number;
    }
    double rounding = 0.5;
    for (uint8_t i=0; i<digits; ++i) rounding /= 10.0;    
    number += rounding;
    unsigned long int_part = (unsigned long)number;
    double remainder = number - (double)int_part;
    p=rxy_intToStr (int_part, p);
    if (digits > 0) {
      *p++ = '.'; 
    }    
    while (digits-- > 0) {
      remainder *= 10.0;
      unsigned int toPrint = (unsigned int)(remainder);
      *p++ = toPrint + '0';
      remainder -= toPrint; 
    } 
    print (buf);    
  }
  
  //void print(const Printable& x);
  
  void println (const __FlashStringHelper * str) {
    print (str);
    println ();
  }

  void println(const char * str) {
    print (str);
    println ();
  }
  
  void println(String str) {
    print (str.c_str());    
    println ();
  }
  
  void println(char c) {
    write (c);
    println ();
  }
  
  void println(unsigned char b , int base = 10) {
    return println((unsigned long) b, base);
  }
  
  void println(int n, int base = 10) {
    return println((long) n, base); 
  }
  
  void println(unsigned int n, int base = 10) {
    return println((unsigned long) n, base); 
  }
  
  void println(long n, int base = 10) {
    print (n, base);
    println (); 
  }
  
  void println(unsigned long n, int base = 10) {
    print (n, base);
    println (); 
  }
  
  void println(double number, int digits = 2) {
    print (number, digits);
    println ();   
  }
  
  //void println(const Printable&);
  
  void println(void) {
    write (0x0d);
    write (0x0a);
  }      

};        
#pragma pack(pop)

#endif // RemoteXYType_Print_h