#ifndef RemoteXYStream_BLEDevice_h
#define RemoteXYStream_BLEDevice_h

#if defined(MAIN_BLEDevice_H_)

#include "RemoteXYComm.h"
#include <BLE2902.h>


#define REMOTEXYCOMM_BLEDEVICE__SEND_BUFFER_SIZE 20
#define REMOTEXYCOMM_BLEDEVICE__RECEIVE_BUFFER_SIZE 1024
#define REMOTEXYCOMM_BLEDEVICE__SERVICE_UUID             "0000FFE0-0000-1000-8000-00805F9B34FB" // UART service UUID
#define REMOTEXYCOMM_BLEDEVICE__CHARACTERISTIC_UUID_RXTX "0000FFE1-0000-1000-8000-00805F9B34FB"

#define REMOTEXYCOMM_BLEDEVICE__SEND_TIME_FOR_ONE_PACKAGE 7  // test min 5 ms
#define REMOTEXYCOMM_BLEDEVICE__SEND_BYTES_BEFORE_OVERFLOW 500  // test max 600 bytes


class CRemoteXYStream_BLEDevice : public CRemoteXYStream, BLEServerCallbacks, BLECharacteristicCallbacks {
  
  protected:
  const char * bleDeviceName;    // need to delete

  BLEServer *pServer;
  BLECharacteristic * pRxTxCharacteristic;

  uint8_t sendBuffer[REMOTEXYCOMM_BLEDEVICE__SEND_BUFFER_SIZE];
  uint16_t sendBufferCount;
  uint16_t sendBytesAvailable;
  
  uint16_t sendCount;
  uint8_t sendBLEBufferOverflow;
  
  uint8_t receiveBuffer[REMOTEXYCOMM_BLEDEVICE__RECEIVE_BUFFER_SIZE];
  uint16_t receiveBufferStart;
  uint16_t receiveBufferPos;
  uint16_t receiveBufferCount;
  
  volatile uint8_t receiveBufferLook; 
    
    
  public:  
  CRemoteXYStream_BLEDevice (const char * _bleDeviceName) : CRemoteXYStream () {
    bleDeviceName = _bleDeviceName;

    receiveBufferLook = 0;         
    receiveBufferCount = 0;        
    receiveBufferStart = 0;
    receiveBufferPos = 0;
    receiveBufferCount = 0;  
        
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("Init ESP32 BLE on chip");
#endif
      // Create the BLE Device
    BLEDevice::init(_bleDeviceName);

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    // Create the BLE Service
    BLEService *pService = pServer->createService(REMOTEXYCOMM_BLEDEVICE__SERVICE_UUID);

    // Create a BLE Characteristic
    pRxTxCharacteristic = pService->createCharacteristic(
                            REMOTEXYCOMM_BLEDEVICE__CHARACTERISTIC_UUID_RXTX,
                            BLECharacteristic::PROPERTY_READ |
                            BLECharacteristic::PROPERTY_NOTIFY |
                            BLECharacteristic::PROPERTY_WRITE_NR 
                          );

    BLE2902 *ble2902 = new BLE2902();
    ble2902->setNotifications(true);
    pRxTxCharacteristic->addDescriptor(ble2902);
    pRxTxCharacteristic->setCallbacks(this);

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->addServiceUUID(pService->getUUID());
    pServer->getAdvertising()->start();

#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("BLE started");
#endif    

  }
                    
  void onConnect(BLEServer* pServer) {
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("BLE client connected");
#endif
    receiveBufferStart = 0;
    receiveBufferPos = 0;
    receiveBufferCount = 0; 
  };  
  
  void onDisconnect(BLEServer* pServer) {
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write("BLE client disconnected");
#endif
    receiveBufferCount = 0;
    pServer->getAdvertising ()->start ();
  }

  void onWrite (BLECharacteristic *pCharacteristic) {
    
#if (ESP_ARDUINO_VERSION_MAJOR >= 3) 
    String rxValue = pCharacteristic->getValue();
#else
    std::string rxValue = pCharacteristic->getValue();
#endif
    if (rxValue.length() > 0) {
      while (receiveBufferLook!=0) { delay(1); } 
      receiveBufferLook=1; 
      for (uint16_t i = 0; i < rxValue.length(); i++) {                
        uint8_t b = (uint8_t)rxValue[i];  
        receiveBuffer[receiveBufferPos++] =  b;
        if (receiveBufferPos >= REMOTEXYCOMM_BLEDEVICE__RECEIVE_BUFFER_SIZE) receiveBufferPos=0; 
        if (receiveBufferCount < REMOTEXYCOMM_BLEDEVICE__RECEIVE_BUFFER_SIZE) receiveBufferCount++;
        else {
          receiveBufferStart++;
          if (receiveBufferStart >= REMOTEXYCOMM_BLEDEVICE__RECEIVE_BUFFER_SIZE) receiveBufferStart=0;
        }       
      } 
      receiveBufferLook=0; 
    }
  }
  
  /*
  // this works already after the buffer overflow and about 100 bytes are lost
  void onStatus (BLECharacteristic *pCharacteristic, Status s, uint32_t code) {
    if (s == Status::SUCCESS_NOTIFY) canWrite = 1; 
  }
  */
  
  void startWrite (uint16_t len) override {
    sendBytesAvailable = len;
    sendBufferCount = 0;
    sendCount = 0;
    sendBLEBufferOverflow = 0;
  }
  
  
  void write (uint8_t b) override {
    // the ESP BLE library have some error
    // it has some sent buffer which can overflow after 600 bytes if sending bytes without delay
    // as a result of the tests, it was found that 5 ms per 20 bytes are needed
    // but if send without delay, it can send up to 600 bytes
  
    sendBuffer[sendBufferCount++] = b;
    sendBytesAvailable--;
    if ((sendBufferCount == REMOTEXYCOMM_BLEDEVICE__SEND_BUFFER_SIZE) || (sendBytesAvailable == 0)) {
      pRxTxCharacteristic->setValue((uint8_t *)sendBuffer, sendBufferCount);
      pRxTxCharacteristic->notify();
      sendCount += sendBufferCount;
      sendBufferCount = 0;
      if (sendCount >= REMOTEXYCOMM_BLEDEVICE__SEND_BYTES_BEFORE_OVERFLOW) {
        if (sendBLEBufferOverflow == 0) {
          delay (REMOTEXYCOMM_BLEDEVICE__SEND_BYTES_BEFORE_OVERFLOW / REMOTEXYCOMM_BLEDEVICE__SEND_BUFFER_SIZE * REMOTEXYCOMM_BLEDEVICE__SEND_TIME_FOR_ONE_PACKAGE);
          sendBLEBufferOverflow = 1;
        }
        delay (REMOTEXYCOMM_BLEDEVICE__SEND_TIME_FOR_ONE_PACKAGE);
      }
    }
  } 
  
  void handler () override {   
    if (receiveBufferCount>0) {
      while (receiveBufferLook!=0) { delay(1); } 
      receiveBufferLook=1; 
      while (receiveBufferCount) {     
        notifyReadByteListener (receiveBuffer[receiveBufferStart++]);
        if (receiveBufferStart >= REMOTEXYCOMM_BLEDEVICE__RECEIVE_BUFFER_SIZE) receiveBufferStart=0;
        receiveBufferCount--;
      }
      receiveBufferLook=0;
    }
  }
  

  
};

#endif // MAIN_BLEDevice_H_
#endif // RemoteXYStream_BLEDevice_h