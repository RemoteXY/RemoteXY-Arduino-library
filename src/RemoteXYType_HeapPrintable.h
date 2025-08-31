#ifndef RemoteXYType_HeapPrintable_h
#define RemoteXYType_HeapPrintable_h

#include "RemoteXYType_Heap.h"


// VIRTUAL
class CRemoteXYTypeInner_HeapPrintable: public CRemoteXYTypeInner_Heap, public Print {
  protected:
  uint16_t printBufferSize;
  uint16_t printBufferLength;
  uint8_t *printBffer;  
  
  public:
  uint8_t* init (uint8_t *conf) override  {
    printBufferSize = 0;
    printBufferLength = 0;
    printBffer = NULL;
    return CRemoteXYTypeInner_Heap::init (conf);
  };
  
  private:
  uint8_t bufferAllocate (uint16_t newSize) {
    newSize = (newSize | 0x000f) + 1; // multiple of 16 bytes
    if (newSize > printBufferSize) {
      if (newSize <= heap.getSize () - 16) { // no more than half of heap
        uint8_t *newBffer = (uint8_t*)realloc (printBffer, newSize);
        if (newBffer != NULL) {
          printBffer = newBffer;
          printBufferSize = newSize;
          return 1;
        }
      }
    }
    return 0;
  }
  
  public:
  size_t write (uint8_t b) override { 
    if (printBufferLength >= printBufferSize) {
      if (bufferAllocate (printBufferSize + 1) == 0) {
        send ();
      }
    }
    if (printBufferLength < printBufferSize) {
      printBffer[printBufferLength++] = b;  
      return 1; 
    }
    return 0;
  } 
  
#if defined(ARDUINO_ARCH_STM32F1) // STM32F1
  size_t write (const void *_buf, uint32 size) override { 
    uint8_t * buf = (uint8_t*)_buf; 
#else
  size_t write (const uint8_t *buf, size_t size) override { 
#endif  
    if (size > 0) {
      if (printBufferLength + size > printBufferSize) {
        if (bufferAllocate (printBufferSize + size) == 0) {
          send ();
        }
      }     
      if (printBufferLength + size <= printBufferSize) {
        while (size--) {
          printBffer[printBufferLength++] = *buf++;
        } 
      }
    }   
    return size; 
  }
  
  void send () {
    if (printBufferLength > 0) {
      if (addBufferToHeap (printBffer, printBufferLength)) {
        printBufferLength = 0;
      }
    }
  }
  
  void clear () {
    printBufferLength = 0;
  }
  
  virtual uint8_t addBufferToHeap (uint8_t *buf, uint16_t len) = 0;
  
};

#define CRemoteXYTypeInner_HeapPrintable_inner ((CRemoteXYTypeInner_HeapPrintable*)inner)

#pragma pack(push, 1) 
class CRemoteXYType_HeapPrintable: public CRemoteXYType {
  
  // Print
  public:
  size_t write (uint8_t b) { 
    return CRemoteXYTypeInner_HeapPrintable_inner->write (b); 
  }  
  size_t write (const char *str) {
    if (str == NULL) return 0;
    return CRemoteXYTypeInner_HeapPrintable_inner->write ((const uint8_t *)str, rxy_strLength(str));
  }
  size_t write (const uint8_t *buffer, size_t size) { 
    return CRemoteXYTypeInner_HeapPrintable_inner->write (buffer, size);   
  }   
  size_t write(const char *buffer, size_t size) {
    return CRemoteXYTypeInner_HeapPrintable_inner->write ((const uint8_t *)buffer, size);   
  }
  size_t print(const __FlashStringHelper * fstr) {
    return CRemoteXYTypeInner_HeapPrintable_inner->print (fstr);
  }
  size_t print(const char* str) {
    return CRemoteXYTypeInner_HeapPrintable_inner->print (str);
  }
  size_t print(char c) {
    return CRemoteXYTypeInner_HeapPrintable_inner->print (c);
  }
  size_t print(unsigned char c, int base = DEC) {
    return CRemoteXYTypeInner_HeapPrintable_inner->print (c, base);
  }
  size_t print(int v, int base = DEC) {
    return CRemoteXYTypeInner_HeapPrintable_inner->print (v, base);
  }
  size_t print(unsigned int v, int base = DEC) {
    return CRemoteXYTypeInner_HeapPrintable_inner->print (v, base);
  }
  size_t print(long v, int base = DEC) {
    return CRemoteXYTypeInner_HeapPrintable_inner->print (v, base);
  }
  size_t print(unsigned long v, int base = DEC) {
    return CRemoteXYTypeInner_HeapPrintable_inner->print (v, base);
  }              
#if defined(ARDUINO)         
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
#else
  size_t print(long long n, int base = DEC) {
    return CRemoteXYTypeInner_HeapPrintable_inner->print (n, base);
  }
  size_t print(unsigned long long n, int base = DEC) {
    return CRemoteXYTypeInner_HeapPrintable_inner->print (n, base);
  }
#endif
 
  size_t print(double v, int d = 2) {
    return CRemoteXYTypeInner_HeapPrintable_inner->print (v, d);
  }
  size_t print(const Printable& p) {
    return CRemoteXYTypeInner_HeapPrintable_inner->print (p);
  }

  size_t println(const __FlashStringHelper * fstr) {
    return CRemoteXYTypeInner_HeapPrintable_inner->println (fstr);
  }
  size_t println(const char* str) {
    return CRemoteXYTypeInner_HeapPrintable_inner->println (str);
  }
  size_t println(char c) {
    return CRemoteXYTypeInner_HeapPrintable_inner->println (c);
  }
  size_t println(unsigned char c, int base = DEC) {
    return CRemoteXYTypeInner_HeapPrintable_inner->println (c, base);
  }
  size_t println(int v, int base = DEC) {
    return CRemoteXYTypeInner_HeapPrintable_inner->println (v, base);
  }
  size_t println(unsigned int v, int base = DEC) {
    return CRemoteXYTypeInner_HeapPrintable_inner->println (v, base);
  }
  size_t println(long v, int base = DEC) {
    return CRemoteXYTypeInner_HeapPrintable_inner->println (v, base);
  }
  size_t println(unsigned long v, int base = DEC) {
    return CRemoteXYTypeInner_HeapPrintable_inner->println (v, base);
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
  size_t println(double v, int d = 2) {
    return CRemoteXYTypeInner_HeapPrintable_inner->println (v, d);
  }
  size_t println(const Printable& p) {
    return CRemoteXYTypeInner_HeapPrintable_inner->println (p);
  }
  size_t println(void) {
    return CRemoteXYTypeInner_HeapPrintable_inner->println ();
  }       
  
#if defined (ARDUINO)

  size_t print(const String &str) {
    return CRemoteXYTypeInner_HeapPrintable_inner->print (str);
  }
  size_t println(const String &str) {
    return CRemoteXYTypeInner_HeapPrintable_inner->println (str);
  }  
  
#endif // ARDUINO

};

#pragma pack(pop)

#endif // RemoteXYType_HeapPrintable_h