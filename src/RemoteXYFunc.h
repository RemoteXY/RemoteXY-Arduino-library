#ifndef RemoteXYFunc_h
#define RemoteXYFunc_h

#include <inttypes.h> 



// INT

char rxy_intToHexChar (uint8_t c) {
  return c < 10 ? c + '0' : c + 'A' - 10;
}

char* rxy_intToFixedStr (uint32_t i, char* s, uint8_t len, char lead, uint8_t base = 10) {
  uint8_t m = len;
  if (base < 2) base = 10;
  while (m--) {
    if (i == 0) s[m] = lead;
    else {
      s[m] = rxy_intToHexChar (i%base);
      i/=base;
    }
  }
  s[len]=0;
  return s+len;
}

char* rxy_intToStr (uint32_t i, char* s, uint8_t base = 10) {
  if (base < 2) base = 10;
  uint8_t bufLen = 32 / (base>>2) + 1;
  char buf[bufLen];
  rxy_intToFixedStr (i, buf, bufLen, 0x20, base);
  char *p = buf;
  while (*p) {
    if (*p != 0x20) *s++ = *p;  
    p++;
  }
  *s = 0;
  return s;  
}

// CHAR

uint8_t rxy_hexCharToInt (char c) {
  uint8_t b;
  if (c<='9') b=c-'0';
  else if (c<='f') b=c-'A'+10; 
  else b=c-'a'+10;
  return b&0x0f;
}

// BUFFERS
// from -> to
void rxy_bufCopy (uint8_t *to, uint8_t *from, uint16_t len) {
  while (len--) *to++ = *from++;
}

void rxy_bufCopy (uint8_t *to1, uint8_t *to2, uint8_t *from, uint16_t len) {
  while (len--) {
    *to1++ = *to2++ = *from++; 
  }
}

uint8_t rxy_bufCompare (uint8_t *p1, uint8_t *p2, uint16_t len) {
  while (len--) {
    if (*p1++ != *p2++) return 0;
  }
  return 1;
}

uint8_t rxy_bufIsEmpty (uint8_t *p, uint16_t len) {
  while (len--) {
    if (*p++ != 0) return 0;
  }
  return 1;
}

void rxy_bufClear (uint8_t *p, uint16_t len) {
  while (len--) *p++ = 0;
}    

// STRINGS


uint16_t rxy_strLength (const char* s) {
  uint16_t len = 0;
  while (*s++) len++; 
  return len; 
}

char* rxy_strCopy (char* to, const char* from) {
  while (*from) *to++ = *from++; 
  *to = *from;  // zero
  return to;  // pointer to zero
}

uint8_t rxy_strCompare (const char* s1, const char* s2) {
  while (*s1 == *s2) {
    if ((*s1++ == 0) && (*s2++ == 0)) return 1;
  }
  return 0;
}

// compares only the first characters of string 1 which should match string 2
uint8_t rxy_strCompareLeft (const char* s1, const char* s2, uint8_t unsensitive) {
  char c1, c2;
  while (*s2 != 0) {
    c1 = *s1++;
    c2 = *s2++;
    if (unsensitive) {
      if ((c1 >= 'a') && (c1 <= 'z')) c1 -= 0x20;
      if ((c2 >= 'a') && (c2 <= 'z')) c2 -= 0x20;
    }
    if (c1 != c2) return 0;
  }
  return 1;
}


char* rxy_strSearchChar (char* s, char c, uint8_t unsensitive) {
  char c1;
  if (*s == 0) return NULL;
  if (unsensitive) {
    if ((c >= 'a') && (c <= 'z')) c -= 0x20;  
  }
  while (*s != 0) {
    c1 = *s;
    if (unsensitive) {
      if ((c1 >= 'a') && (c1 <= 'z')) c1 -= 0x20;
    }
    if (c1 == c) return s;
    s++;
  }
  return NULL;
}

char* rxy_strSearch (char* s, char* needle, uint8_t unsensitive) {
  if (*needle == 0) return NULL;
  s = rxy_strSearchChar (s, *needle, unsensitive);
  while (s) {
    if (rxy_strCompareLeft (s, needle, unsensitive)) return s;
    s = rxy_strSearchChar (s, *needle, unsensitive);
  }
  return NULL;
}

char* rxy_strParseInt (const char* s, uint16_t * val) {
  *val = 0;
  uint8_t f = 0;
  char c;
  while (*s != 0) {
    c = *s++;
    if ((c == 0x20) && (f == 0)) continue; // left spases
    if ((c >= 0x30) && (c <= 0x39)) {
      *val *= 10;
      *val += c - 0x30;
      f=1;
    }
    else break;
  }
  return (char*)s;
}

void rxy_strParseMacAddr (const char* s, uint8_t* m) {
  uint8_t i, b;
  for (i=6; i>0; i--) {
    b=rxy_hexCharToInt (*s++)<<4;
    b|=rxy_hexCharToInt (*s++);
    *m++=b;
    s++;
  }    
}


// CRC
#define REMOTEXY_INIT_CRC 0xffff

void rxy_initCRC (uint16_t *crc) {
  *crc = REMOTEXY_INIT_CRC;
}

void rxy_updateCRC (uint16_t *crc, uint8_t b) {
  *crc ^= b;
  for (uint8_t i=0; i<8; ++i) {
    if ((*crc) & 1) *crc = ((*crc) >> 1) ^ 0xA001;
    else *crc >>= 1;
  }
} 



#endif //RemoteXYFunc_h