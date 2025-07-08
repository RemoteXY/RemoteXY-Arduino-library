#ifndef RemoteXYStream_ArduinoBLE_h
#define RemoteXYStream_ArduinoBLE_h
                          
#if defined(_ARDUINO_BLE_H_) 

#include "RemoteXYStream.h"
                                                         
#define REMOTEXYSTREAM_ARDUINOBLE__SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // NORDIC UART service UUID
#define REMOTEXYSTREAM_ARDUINOBLE__RX_CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"    
#define REMOTEXYSTREAM_ARDUINOBLE__TX_CHARACTERISTIC_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"    
#define REMOTEXYSTREAM_ARDUINOBLE__CHARACTERISTIC_SIZE 20  
#define REMOTEXYSTREAM_ARDUINOBLE__RECEIVE_BUFFER_SIZE 1024
#define REMOTEXYSTREAM_ARDUINOBLE__SEND_TIME_FOR_ONE_PACKAGE 10  // test min 8 ms for iOS, 5 ms for Android
#define REMOTEXYSTREAM_ARDUINOBLE__SEND_BYTES_BEFORE_OVERFLOW 500  // test max xxx bytes


class CRemoteXYStream_ArduinoBLE;
CRemoteXYStream_ArduinoBLE * CRemoteXYStream_ArduinoBLE_instance = NULL;
void CRemoteXYStream_ArduinoBLE_onBLEWritten (BLEDevice central, BLECharacteristic characteristic);


class CRemoteXYStream_ArduinoBLE : public CRemoteXYStream {

  private:
  
  uint8_t failed;
  uint8_t connected;
  
  BLEService * service;
  BLECharacteristic * rxCharacteristic;
  BLECharacteristic * txCharacteristic;
  BLEDescriptor * txDescriptor;
    
  uint8_t sendBuffer[REMOTEXYSTREAM_ARDUINOBLE__CHARACTERISTIC_SIZE];
  uint16_t sendBufferCount;

  
  uint8_t receiveBuffer[REMOTEXYSTREAM_ARDUINOBLE__RECEIVE_BUFFER_SIZE];
  uint16_t receiveBufferStart;
  uint16_t receiveBufferPos;
  uint16_t receiveBufferCount;
  
  
  public:
  CRemoteXYStream_ArduinoBLE (const char * _bleDeviceName) : CRemoteXYStream () { 
    CRemoteXYStream_ArduinoBLE_instance = this;
    sendBufferCount = 0;

#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("Init Arduino BLE"));
#endif
    connected = 0;

    if (!BLE.begin()) {
      failed = 1;
#if defined(REMOTEXY__DEBUGLOG)
      RemoteXYDebugLog.write(F("Starting BLE module failed"));
#endif    
      return;
    }
    
    receiveBufferCount = 0;        
    receiveBufferStart = 0;
    receiveBufferPos = 0;

    
    BLE.setLocalName(_bleDeviceName);
    BLE.setDeviceName(_bleDeviceName);
    service = new BLEService(REMOTEXYSTREAM_ARDUINOBLE__SERVICE_UUID); 
    BLE.setAdvertisedService(*service);
    rxCharacteristic =  new BLECharacteristic(REMOTEXYSTREAM_ARDUINOBLE__RX_CHARACTERISTIC_UUID, BLEWriteWithoutResponse, REMOTEXYSTREAM_ARDUINOBLE__CHARACTERISTIC_SIZE);   
    txCharacteristic =  new BLECharacteristic(REMOTEXYSTREAM_ARDUINOBLE__TX_CHARACTERISTIC_UUID, BLENotify, REMOTEXYSTREAM_ARDUINOBLE__CHARACTERISTIC_SIZE);   
    uint8_t descriptorValue[2] = {00, 00};
    txDescriptor = new BLEDescriptor ("2902", descriptorValue, sizeof (descriptorValue));
    txCharacteristic->addDescriptor (*txDescriptor);
    
    service->addCharacteristic(*rxCharacteristic);
    service->addCharacteristic(*txCharacteristic);
    
    rxCharacteristic->setEventHandler(BLEWritten, CRemoteXYStream_ArduinoBLE_onBLEWritten);

    BLE.addService(*service);

    rxCharacteristic->writeValue((uint8_t)0);
    txCharacteristic->writeValue((uint8_t)0);
    BLE.advertise();

        
#if defined(REMOTEXY__DEBUGLOG)
    RemoteXYDebugLog.write(F("Arduino BLE started"));
#endif  
    failed = 0;

  }          
  
    
    
  void write (uint8_t b) override {
    if (failed !=0) return;      
    sendBuffer[sendBufferCount++] = b;
    if (sendBufferCount == REMOTEXYSTREAM_ARDUINOBLE__CHARACTERISTIC_SIZE) {
      flush ();     
    }
  }     
  
  void flush () override {
    if (sendBufferCount > 0) {
      if (connected != 0) {            
        txCharacteristic->writeValue(sendBuffer, sendBufferCount);   
        BLE.poll();   
        delay (REMOTEXYSTREAM_ARDUINOBLE__SEND_TIME_FOR_ONE_PACKAGE);  
      }    
      sendBufferCount = 0; 
    }       
  }

  void handler () override {       
    if (failed !=0) return;
    if (BLE.connected()) {
      if (connected == 0) {
        receiveBufferCount = 0;        
        receiveBufferStart = 0;
        receiveBufferPos = 0;
        connected = 1;
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write(F("BLE client connected"));
#endif          
      }
    }
    else {
      if (connected != 0) {
        connected = 0;
#if defined(REMOTEXY__DEBUGLOG)
        RemoteXYDebugLog.write(F("BLE client disconnected"));
#endif        
      }
    }
    
    uint8_t b;  
    BLE.poll();
    while (receiveBufferCount > 0) {     
      b =  receiveBuffer[receiveBufferStart++];
      if (receiveBufferStart >= REMOTEXYSTREAM_ARDUINOBLE__RECEIVE_BUFFER_SIZE) receiveBufferStart=0;
      receiveBufferCount--;
      notifyReadByteListener (b);
    }
    
    /*
    BLE.poll();
    if (rxCharacteristic->written ()) {
      uint8_t buff[REMOTEXYSTREAM_ARDUINOBLE__CHARACTERISTIC_SIZE];
      uint8_t cnt = rxCharacteristic->readValue (buff, REMOTEXYSTREAM_ARDUINOBLE__CHARACTERISTIC_SIZE);    
      for (uint8_t i=0; i<cnt; i++) {     
        notifyReadByteListener (buff[i]);
      }
    }
    */
  } 
  
  public:
  void onBLEWritten () {
    uint8_t buff[REMOTEXYSTREAM_ARDUINOBLE__CHARACTERISTIC_SIZE];
    uint8_t cnt = rxCharacteristic->readValue (buff, REMOTEXYSTREAM_ARDUINOBLE__CHARACTERISTIC_SIZE);    

    for (uint8_t i = 0; i < cnt; i++) {                
      receiveBuffer[receiveBufferPos++] =  buff[i];
      if (receiveBufferPos >= REMOTEXYSTREAM_ARDUINOBLE__RECEIVE_BUFFER_SIZE) receiveBufferPos=0; 
      if (receiveBufferCount < REMOTEXYSTREAM_ARDUINOBLE__RECEIVE_BUFFER_SIZE) receiveBufferCount++;
      else {
        receiveBufferStart++;
        if (receiveBufferStart >= REMOTEXYSTREAM_ARDUINOBLE__RECEIVE_BUFFER_SIZE) receiveBufferStart=0;
      }       
    } 
  }   
};


void CRemoteXYStream_ArduinoBLE_onBLEWritten (BLEDevice central, BLECharacteristic characteristic) {
  if (CRemoteXYStream_ArduinoBLE_instance != NULL) {
    CRemoteXYStream_ArduinoBLE_instance->onBLEWritten ();
  }
}



#endif  // _ARDUINO_BLE_H_

#endif //RemoteXYStream_ArduinoBLE_h