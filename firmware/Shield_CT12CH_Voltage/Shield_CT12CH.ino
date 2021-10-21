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

#include <Arduino.h>
#include "EmonLib.h"


/* code for SAMD21G18A-AUT - 14 ADC channels 
 *  Pin  3 PA02_AIN[0]
 *  Pin  4 PA03_AREFA (AIN[1] not in arduino?) -> VREF
 *  Pin  7 PB08_AIN[2]
 *  Pin  8 PB09_AIN[3]
 *  Pin  9 PA04_AIN[4]
 *  Pin 10 PA05_AIN[5]
 *  Pin 11 PA06_AIN[6]  (not in arduino?) -> D8
 *  Pin 12 PA07_AIN[7]  (not in arduino?) -> D10
 *  Pin 13 PA08_AIN[16] (not in arduino?) -> D4
 *  Pin 14 PA09_AIN[17] (not in arduino?) -> D3
 *  Pin 15 PA10_AIN[18] (not in arduino?) -> D1
 *  Pin 16 PA11_AIN[19] (not in arduino?) -> D0
 *  Pin 47 PB02_AIN[10]
 *  Pin 48 PB03_AIN[11] (not in arduino?) -> LED 
*/

/* defs
  #define PIN_A0               (14ul)
  #define PIN_A1               (15ul)
  #define PIN_A2               (16ul)
  #define PIN_A3               (17ul)
  #define PIN_A4               (18ul)
  #define PIN_A5               (19ul)

 * | 14         | A0               |  PA02  | A0              | EIC/EXTINT[2] *ADC/AIN[0]  DAC/VOUT  PTC/Y[0]
 * | 15         | A1               |  PB08  | A1              | EIC/EXTINT[8] *ADC/AIN[2]            PTC/Y[14] SERCOM4/PAD[0]                  TC4/WO[0]
 * | 16         | A2               |  PB09  | A2              | EIC/EXTINT[9] *ADC/AIN[3]            PTC/Y[15] SERCOM4/PAD[1]                  TC4/WO[1]
 * | 17         | A3               |  PA04  | A3              | EIC/EXTINT[4] *ADC/AIN[4]  AC/AIN[0] PTC/Y[2]  SERCOM0/PAD[0]                  TCC0/WO[0]
 * | 18         | A4               |  PA05  | A4              | EIC/EXTINT[5] *ADC/AIN[5]  AC/AIN[1] PTC/Y[5]  SERCOM0/PAD[1]                  TCC0/WO[1]
 * | 19         | A5               |  PB02  | A5              | EIC/EXTINT[2] *ADC/AIN[10]           PTC/Y[8]  SERCOM5/PAD[0]

  { PORTA,  2, PIO_ANALOG, PIN_ATTR_ANALOG, ADC_Channel0, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_2 }, // ADC/AIN[0]
  { PORTB,  8, PIO_ANALOG, (PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel2, PWM4_CH0, TC4_CH0, EXTERNAL_INT_8 }, // ADC/AIN[2]
  { PORTB,  9, PIO_ANALOG, (PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel3, PWM4_CH1, TC4_CH1, EXTERNAL_INT_9 }, // ADC/AIN[3]
  { PORTA,  4, PIO_ANALOG, 0, ADC_Channel4, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_4 }, // ADC/AIN[4]
  { PORTA,  5, PIO_ANALOG, 0, ADC_Channel5, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_5 }, // ADC/AIN[5]
  { PORTB,  2, PIO_ANALOG, 0, ADC_Channel10, NOT_ON_PWM, NOT_ON_TIMER, EXTERNAL_INT_2 }, // ADC/AIN[10]
*/

/*  Time (ms) to allow the filters to settle before sending data */
#define FILTERSETTLETIME 5000                                           

/* Set to 0 to disable */
#define CT_CH_CNT 1

// Create  instances for each CT channel
EnergyMonitor ct[CT_CH_CNT];

const int LEDpin = 13;         // On-board emonTx LED 
const int nodeID = 5;          // emonTx node ID
boolean settled = false;

void setup() 
{
  SerialUSB.begin(9600);
  SerialUSB.println("emonTX Shield CT-12-CH + AC"); 
  SerialUSB.println("OpenEnergyMonitor.org");
  SerialUSB.print("Node: "); 
  SerialUSB.print(nodeID); 

  analogReadResolution(12);

  //3v3, 3.3v pullup - now at least the current factor makes sense

  for(int i = 0; i < CT_CH_CNT; i++) {
    ct[i].voltage(A0, 206.5, 1.7);  // Voltage: input pin, calibration, phase_shift
    ct[i].current(A1+1, 13.5);      // Current: input pin, calibration.
    pinMode(A0 + i, INPUT);
  }
 
  // Setup indicator LED
  pinMode(LEDpin, OUTPUT);                                            
  digitalWrite(LEDpin, HIGH);
}

void loop() 
{ 
  for(int i = 0; i < CT_CH_CNT; i++) {
    ct[i].calcVI(20,2000);
    ct[i].serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
    //SerialUSB.print(emontx.power1);                                         
  }
  SerialUSB.println(); delay(100);

  // because millis() returns to zero after 50 days ! 
  if (!settled && millis() > FILTERSETTLETIME) settled = true;

  if (settled)                                                         // send data only after filters have settled
  { 
    digitalWrite(LEDpin, HIGH); delay(2); digitalWrite(LEDpin, LOW);   // flash LED
    delay(2000);                                                       // delay between readings in ms
  }
}
