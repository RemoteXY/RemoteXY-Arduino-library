#ifndef RemoteXYEeprom_h
#define RemoteXYEeprom_h

#if defined REMOTEXY_HAS_EEPROM 


#ifndef REMOTEXY_EEPROM_OFFSET
#define REMOTEXY_EEPROM_OFFSET 0
#endif


#if defined (ESP8266) || defined (ESP32) 
#define REMOTEXY_EEPROM_ESP
#endif


#define REMOTEXY_BOARDID_EEPROM_KEY 0xff10


class CRemoteXYEepromItem {
  public:
  CRemoteXYEepromItem * next;
  uint16_t address; // address in eeprom
  uint8_t * comparedData; 
  uint8_t * data; // copy of data from eeprom
  uint16_t size;
  uint16_t key;  // any key
  
  // Automatic change control via handler()
  public:
  CRemoteXYEepromItem (uint8_t * _comparedData, uint16_t _size, uint16_t _key) {
    data = NULL;
    comparedData = _comparedData;
    size = _size;
    key = _key;
  }
  
  // Manual control
  public:
  CRemoteXYEepromItem (uint16_t _size, uint16_t _key) {
    data = NULL;
    comparedData = NULL;
    size = _size;
    key = _key;
  }
};

class CRemoteXYEeprom {

  private:
  uint16_t offset;
  uint16_t size;
  uint8_t * data;
  uint16_t keyAddress;
  CRemoteXYEepromItem * items;
  
  public:
  uint8_t initialized;
  
  
  public:
  CRemoteXYEeprom () {
    items = NULL;
    offset = REMOTEXY_EEPROM_OFFSET;
    initialized = 0;  
  }
  
  void setOffset (uint16_t _offset) {
    offset = _offset;
  }
  
  // Automatic change control via handler()
  public: 
  void addItem (uint8_t * data, uint16_t size, uint16_t key) {
    RemoteXYEepromItem * item = new RemoteXYEepromItem (data, size, key);
    item->next = items;
    items = item;
  } 
  
  // Manual writing using writeItem()
  public: 
  CRemoteXYEepromItem * addItem (uint16_t size, uint16_t key) {
    CRemoteXYEepromItem * item = new RemoteXYEepromItem (size, key);
    item->next = items;
    items = item;
    return item;
  } 
  
  public: 
  uint16_t getSize () {
    uint16_t sz = 0;
    RemoteXYEepromItem * item = items;
    while (item) {
      sz += item->size;
      item = item->next;
    }
    return sz+2;    
  }
  
  
  public: 
  uint8_t init (uint8_t callBegin) {
    size = 0;
    CRemoteXYEepromItem * item = items;
    while (item) {
      size += item->size;
      item = item->next;
    }
    if (size > 0) {
      uint8_t address = 0;
      data = (uint8_t *) malloc (size);
      if (data == NULL) {
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.init ();
        RemoteXYDebugLog.write(F("Out of RAM memory for EEPROM support: "));
        RemoteXYDebugLog.writeAdd(size);
#endif 
        return 0;
      }
      item = items;
      uint8_t * itemData = data;
      while (item) {
        item->data = itemData;
        item->address = address;
        readBuf (address, itemData, item->size);
        itemData += item->size;
        address += item->size;         
        item = item->next;
      }
      keyAddress = address;
      size += 2; 
          
      if (callBegin) begin (size);
 
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write(F("EEPROM started, size "));
      RemoteXYDebugLog.writeAdd(size);
      RemoteXYDebugLog.writeAdd(F(" offset "));
      RemoteXYDebugLog.writeAdd(offset);
#endif       
      
      uint16_t key = getKey ();
      item = items;
      if (readWord (keyAddress) == key) {
        // eeprom has actual data
        while (item) {
          if (item->comparedData != NULL) {
            rxy_bufCopy (item->comparedData, item->data, item->size);
          }
          item = item->next;
        }        
      }
      else {
        // eeprom data is invalid
        while (item) {
          if (item->comparedData != NULL) {
            rxy_bufCopy (item->data, item->comparedData, item->size);
          }
          else {
            rxy_bufClear (item->data, item->size);
          }
          writeBuf (item->address, item->data, item->size);
          item = item->next;         
        }       
        writeWord (keyAddress, key);    
        commit ();   
      }
    }
    initialized = 1;
    return 1;
  } 
  
  
  public:
  void handler () {
    uint8_t *pd, *pcd;
    uint16_t addr, cnt;
    CRemoteXYEepromItem * item = items;
    uint8_t changed = 0;
    while (item) {
      if ((item->comparedData != NULL) && (item->data != NULL)) {
        if (rxy_bufCompare (item->comparedData, item->data, item->size) == 0) {
          pd = item->data;
          pcd = item->comparedData;
          addr = item->address;
          cnt = item->size;
          while (cnt--) {
            if (*pd != *pcd) {
              *pd = *pcd;
              write (addr, *pd);
              changed = 1;
            }
            pd++;
            pcd++;
            addr++;            
          }         
        }  
      }    
      item = item->next;
    }
    if (changed) {
      writeWord (keyAddress, getKey ());    
      commit ();     
    }   
  }
  
  public:
  void writeItem (CRemoteXYEepromItem * item) {
    if (item->data != NULL) {
      uint8_t * pd = item->data;
      uint16_t addr = item->address;
      uint16_t cnt = item->size;
      uint8_t changed = 0;
      while (cnt--) {
        if (*pd != read (addr)) {
          write (addr, *pd);
          changed = 1;
        }
        pd++;
        addr++;            
      }         
      if (changed) {
        writeWord (keyAddress, getKey ());    
        commit ();     
      }   
    } 
  }
  
  private:
  uint16_t getKey () {
    uint16_t crc;
    rxy_initCRC (&crc);
    
    // first part
    uint8_t * p = data;
    uint16_t sz = size;
    while (sz--) rxy_updateCRC (&crc, *p++);
    
    // second part
    CRemoteXYEepromItem * item = items;
    while (item) {
      rxy_updateCRC (&crc, item->key & 0xff);
      rxy_updateCRC (&crc, (item->key >> 8) & 0xff);
      item = item->next;
    }
    return crc;
  }
   
  private:
  void begin (uint16_t size) {
#if defined (REMOTEXY_EEPROM_ESP)
    EEPROM.begin (size);
#endif   
  }
  
  private:
  uint8_t read (uint16_t address) {
    return EEPROM.read (address + offset);
  }
  
  private:
  void write (uint16_t address, uint8_t value) {
    EEPROM.write (address + offset, value);
  }
  
  // call always after writing
  private:
  void commit () {
#if defined (REMOTEXY_EEPROM_ESP)
    EEPROM.commit ();
#endif    
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write(F("EEPROM written"));
#endif   
  }
  
  private:
  uint16_t readWord (uint16_t address) {
    return read (address) | (read (address + 1) << 8);
  }
  
  private:
  void writeWord (uint16_t address, uint16_t value) {
    write (address, value & 0xff);
    write (address +1, (value >> 8) & 0xff);
  }
  

  private:
  void readBuf (uint16_t address, uint8_t * buf, uint16_t len) {
    while (len--) {
      *buf++ = read (address++);
    }
  }
  
  private:  
  void writeBuf (uint16_t address, uint8_t * buf, uint16_t len) {
    while (len--) {
      write (address++, *buf++);
    }
  }  
      
};


#endif // REMOTEXY_HAS_EEPROM
#endif // RemoteXYEeprom_h