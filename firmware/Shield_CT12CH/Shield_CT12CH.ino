/*
 EmonTx Shield 12x CT example
 
 An example sketch for the emontx Arduino shield module for CT only electricity monitoring.
 
 Licence: GNU GPL V3
 
 Authors: Glyn Hudson, Trystan Lea
 Builds upon JeeLabs RF12 library and Arduino
 
 emonTx documentation: 	http://openenergymonitor.org/emon/modules/emontxshield/
 emonTx firmware code explination: http://openenergymonitor.org/emon/modules/emontx/firmware
 emonTx calibration instructions: http://openenergymonitor.org/emon/modules/emontx/firmware/calibration

 THIS SKETCH REQUIRES:

 Libraries in the standard arduino libraries folder:
	- JeeLib		https://github.com/jcw/jeelib
	- EmonLib		https://github.com/openenergymonitor/EmonLib.git

 Other files in project directory (should appear in the arduino tabs above)
	- emontx_lib.ino
*/

/*Recommended node ID allocation
------------------------------------------------------------------------------------------------------------
-ID-	-Node Type- 
0	- Special allocation in JeeLib RFM12 driver - reserved for OOK use
1-4     - Control nodes 
5-10	- Energy monitoring nodes
11-14	--Un-assigned --
15-16	- Base Station & logging nodes
17-30	- Environmental sensing nodes (temperature humidity etc.)
31	- Special allocation in JeeLib RFM12 driver - Node31 can communicate with nodes on any network group
-------------------------------------------------------------------------------------------------------------
*/

#define FILTERSETTLETIME 5000                                           //  Time (ms) to allow the filters to settle before sending data

const int CT1 = 1; 
const int CT2 = 1;                                                      // Set to 0 to disable 
const int CT3 = 1;
const int CT4 = 1;
const int CT5 = 1;
const int CT6 = 1;
const int CT7 = 1;
const int CT8 = 1;
const int CT9 = 1;
const int CT10 = 1;
const int CT11 = 1;
const int CT12 = 1;

//#include <JeeLib.h>                                                    // make sure V12 (latest) is used if using RFM69CW
#include "EmonLib.h"
EnergyMonitor ct1,ct2,ct3,ct4,ct5,ct6,ct7,ct8,ct9,ct10,ct11,ct12;      // Create  instances for each CT channel

// create structure - a neat way of packaging data for RF comms, nothing is added a a 5th integer to match data structure of voltage version
typedef struct { int power1, power2, power3, power4, power5, power6, power7, power8, power9, power10, power11, power12, nothing; } PayloadTX; 
PayloadTX emontx;                                                       

const int LEDpin = 13;         // On-board emonTx LED 
const int nodeID = 6;          // emonTx RFM12B node ID
const int networkGroup = 210;  // emonTx RFM12B wireless network group - needs to be same as emonBase and emonGLC
boolean settled = false;

void setup() 
{
  Serial.begin(9600);
  Serial.println("emonTX Shield CT-12-CH"); 
  Serial.println("OpenEnergyMonitor.org");
  Serial.print("Node: "); 
  Serial.print(nodeID); 
  Serial.print(" Network: "); 
  Serial.println(networkGroup);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);
             
  if (CT1) ct1.current(1, 60.606);                                     // Setup emonTX CT channel (channel, calibration)
  if (CT2) ct2.current(2, 60.606);                                     // Calibration factor = CT ratio / burden resistance
  if (CT3) ct3.current(3, 60.606); 
  if (CT4) ct4.current(4, 60.606); 
  if (CT5) ct5.current(5, 60.606); 
  if (CT6) ct6.current(6, 60.606); 
  if (CT7) ct7.current(7, 60.606); 
  if (CT8) ct8.current(8, 60.606); 
  if (CT9) ct9.current(9, 60.606); 
  if (CT10) ct10.current(10, 60.606); 
  if (CT11) ct11.current(11, 60.606); 
  if (CT12) ct12.current(12, 60.606); 
  
  // emonTx Shield Calibration = (100A / 0.05A) / 33 Ohms
  pinMode(LEDpin, OUTPUT);                                             // Setup indicator LED
  digitalWrite(LEDpin, HIGH);
  
}

void loop() 
{ 
  if (CT1) {
    emontx.power1 = ct1.calcIrms(1480) * 240.0;                        //ct.calcIrms(number of wavelengths sample)*AC RMS voltage
    Serial.print(emontx.power1);                                         
  }
  if (CT2) {
    emontx.power2 = ct2.calcIrms(1480) * 240.0;
    Serial.print(" "); Serial.print(emontx.power2);
  } 
  if (CT3) {
    emontx.power3 = ct3.calcIrms(1480) * 240.0;
    Serial.print(" "); Serial.print(emontx.power3);
  } 
  if (CT4) {
    emontx.power4 = ct4.calcIrms(1480) * 240.0;
    Serial.print(" "); Serial.print(emontx.power4);
  }
  if (CT5) {
    emontx.power4 = ct5.calcIrms(1480) * 240.0;
    Serial.print(" "); Serial.print(emontx.power4);
  }
  if (CT6) {
    emontx.power4 = ct6.calcIrms(1480) * 240.0;
    Serial.print(" "); Serial.print(emontx.power4);
  }
  if (CT7) {
    emontx.power4 = ct7.calcIrms(1480) * 240.0;
    Serial.print(" "); Serial.print(emontx.power4);
  }
  if (CT8) {
    emontx.power4 = ct8.calcIrms(1480) * 240.0;
    Serial.print(" "); Serial.print(emontx.power4);
  }
  if (CT9) {
    emontx.power4 = ct9.calcIrms(1480) * 240.0;
    Serial.print(" "); Serial.print(emontx.power4);
  }
  if (CT10) {
    emontx.power4 = ct10.calcIrms(1480) * 240.0;
    Serial.print(" "); Serial.print(emontx.power4);
  }
  if (CT11) {
    emontx.power4 = ct11.calcIrms(1480) * 240.0;
    Serial.print(" "); Serial.print(emontx.power4);
  }
  if (CT12) {
    emontx.power4 = ct12.calcIrms(1480) * 240.0;
    Serial.print(" "); Serial.print(emontx.power4);
  }
  Serial.println(); delay(100);

  // because millis() returns to zero after 50 days ! 
  if (!settled && millis() > FILTERSETTLETIME) settled = true;

  if (settled)                                                         // send data only after filters have settled
  { 
    digitalWrite(LEDpin, HIGH); delay(2); digitalWrite(LEDpin, LOW);   // flash LED
    delay(2000);                                                       // delay between readings in ms
  }
}
