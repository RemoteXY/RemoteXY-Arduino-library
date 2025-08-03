#ifndef RemoteXY_ArduinoCompat_Print_h
#define RemoteXY_ArduinoCompat_Print_h

#include "../RemoteXYFunc.h"

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2

class Print;

class Printable {
  public:
  virtual size_t printTo (Print& p) const = 0;
};

class Print {

  public:
  virtual size_t write (uint8_t b) = 0;
  virtual size_t write (const uint8_t *buffer, size_t size) = 0; 
       
  size_t write(const char *buffer, size_t size) {
    return write ((const uint8_t *)buffer, size);
  }  
   
  size_t print () {
    return 0;
  }
      
  size_t print (const __FlashStringHelper * str) {
    PGM_P p = reinterpret_cast<PGM_P>(str);
    uint8_t c;
    size_t len = 0;
    while (1) {
      c = pgm_read_byte(p++);
      if (c == 0) break;
      write(c);  
      len++;   
    }
    return len;
  }
  
       
  size_t print(const char * str) {
    if (str != NULL) {
      return write ((const uint8_t*)str, rxy_strLength (str));
    }
    return 0;
  }
  
  //size_t print(String str) {
  //  return print (str.c_str());
  //}
  
  size_t print(char c) {
    return write (c);
  }
  
  size_t print(unsigned char b, int base = DEC) {
    return print((unsigned long) b, base);
  }
  
  size_t print(int n, int base = DEC) {
    return print((long) n, base); 
  }
  
  size_t print(unsigned int n, int base = DEC) {
    return print((unsigned long) n, base);
  }
  
  size_t print(long n, int base = DEC) {
    char buf[8 * sizeof (uint32_t) + 2];
    char *p = buf;    
    if (n < 0) {
      *p++ = '-';
      n = -n;
    }
    rxy_intToStr (n, p, base);
    return print (buf);
  }
  
  size_t print(unsigned long n, int base = DEC) {
    char buf[8 * sizeof (uint32_t) + 1];
    rxy_intToStr (n, buf, base);
    return print (buf);
  }
  
  size_t print(long long n, int base = DEC) {
    char buf[8 * sizeof (uint64_t) + 2];
    char *p = buf;    
    if (n < 0) {
      *p++ = '-';
      n = -n;
    }
    rxy_uint64ToStr (n, p, base);
    return print (buf);
  }
  
  size_t print(unsigned long long n, int base = DEC) {
    char buf[8 * sizeof (uint64_t) + 1];  
    rxy_uint64ToStr (n, buf, base);
    return print (buf);
  }  
  
  size_t print(double number, int digits = 2) {
    size_t len = 0;
    if (number < 0) {
      len += write ('-');
      number = -number;
    }
    double rounding = 0.5;
    for (uint8_t i=0; i<digits; ++i) rounding /= 10.0;    
    number += rounding;
    unsigned long int_part = (unsigned long)number;
    double remainder = number - (double)int_part;
    
    len += print (int_part);
    if (digits > 0) {
      len += write ('.'); 
    }    
    while (digits-- > 0) {
      remainder *= 10.0;
      uint8_t toPrint = (uint8_t)(remainder);
      len += write (toPrint + '0');
      remainder -= toPrint; 
    }    
    return len;
  }
  
  size_t print (const Printable& p) {
    return p.printTo (*this);
  }  
  
  size_t println (const __FlashStringHelper * str) {
    size_t len = print (str);
    len += println ();
    return len;
  }

  size_t println(const char * str) {
    size_t len = print (str);
    len += println ();
    return len;
  }
  
  size_t println(char c) {
    size_t len = write (c);
    len += println ();
    return len;
  }
  
  size_t println(unsigned char b , int base = DEC) {
    return println((unsigned long) b, base);
  }
  
  size_t println(int n, int base = DEC) {
    return println((long) n, base); 
  }
  
  size_t println(unsigned int n, int base = DEC) {
    return println((unsigned long) n, base); 
  }
  
  size_t println(long n, int base = DEC) {
    size_t len = print (n, base);
    len += println (); 
    return len;
  }
  
  size_t println(unsigned long n, int base = DEC) {
    size_t len = print (n, base);
    len += println (); 
    return len;
  }
  
  size_t println(long long n, int base = DEC) {
    size_t len = print (n, base);
    len += println (); 
    return len;
  }
  
  size_t println(unsigned long long n, int base = DEC) {
    size_t len = print (n, base);
    len += println (); 
    return len;
  }
  
  size_t println(double number, int digits = 2) {
    size_t len = print (number, digits);
    len += println ();  
    return len; 
  }
  
  size_t println (const Printable& p) {
    size_t len = p.printTo (*this);
    len += println ();  
    return len; 
  } 
  
  size_t println(void) {
    size_t len = write (0x0d);
    len += write (0x0a);
    return 2;
  }      

};        






#endif // RemoteXY_ArduinoCompat_Print_h