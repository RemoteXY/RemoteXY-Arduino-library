#ifndef RemoteXYType_Print_h
#define RemoteXYType_Print_h

#include "RemoteXYFunc.h"
#include "RemoteXYType_Message.h"

class RemoteXYType_Print : public RemoteXYType_Message {
  
  public:
  uint16_t sizeOf () override {
    return sizeof (RemoteXYType_Print);
  }
      
  void print(const char * str) {
    send (str);
  }
  
  void print(char c) {
    send (&c, 1);
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
    uint8_t bufLen = 32 / (base>>2) + 2;
    char buf[bufLen];
    char *p = buf;    
    if (n < 0) {
      *p++ = '-';
      n = -n;
    }
    rxy_intToStr (n, p, base);
    send (buf);
  }
  
  void print(unsigned long n, int base = 10) {
    if (base < 2) base = 10;
    uint8_t bufLen = 32 / (base>>2) + 1;
    char buf[bufLen];
    rxy_intToStr (n, buf, base);
    send (buf);
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
    send (buf);    
  }
  
  //void print(const Printable& x);

  void println(const char * str) {
    send (str);
    println ();
  }
  
  void println(char c) {
    send (&c, 1);
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
    send("\r\n");
  }      
    
   
};




#endif // RemoteXYType_Print_h