#include <Arduino.h>

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// RemoteXY select connection mode and include library 
//#define REMOTEXY__DEBUGLOGS Serial

// -----------------------SUGGESTED CODE FROM REMOTEXY FORUM -------------------------
// #define REMOTEXY_MODE__SOFTSERIAL
// #include <SoftwareSerial.h>
// #include <RemoteXY.h>

// // RemoteXY connection settings 
// #define REMOTEXY_SERIAL_RX 0 // or try 1
// #define REMOTEXY_SERIAL_TX 1  // or try 0
// #define REMOTEXY_SERIAL_SPEED 9600

// // RemoteXY select connection mode and include library 
// #define REMOTEXY_MODE__HARDSERIAL
#define REMOTEXY_MODE__BLE_NANO

#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 9600


// ----------------------------------------------------------------------------

// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =
  { 255,1,0,0,0,13,0,11,13,0,
  1,0,42,24,12,12,2,31,88,0 }; 
  
// this structure defines all the variables and events of your control interface 
struct {

    // output variables
  uint8_t button_1; // =1 if button pressed, else =0 
    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
///////////////////////////////////////////// 


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.println("INITIALIZING");

  RemoteXY_Init ();  

}

void loop() {

  RemoteXY_Handler ();

  // uint8_t bte = 0xA;
  // Serial.print(bte);


  // put your main code here, to run repeatedly:
}
