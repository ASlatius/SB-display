#include <Arduino.h>
#include "SoftwareSerial.h"
#include "HC12.h"
#include "CRC32.h"

#include "display.h"

SoftwareSerial HC12Serial(7,6);
HC12 hc12(&HC12Serial, 8, 9600);
int guest=0,home=0,time=0;
String rxData;

bool parseRxData(String rxData, int* pGuest, int* pHome, int* pTime)
{
  String part, debug;
  CRC32 crc;
  int f=0,t=0,i=0;
  uint32_t val;

  //Serial.print(rxData);

  crc.reset();
  while (t >= 0) {
    t = rxData.indexOf(";", f);
    val = rxData.substring(f, t).toInt();

    // Serial.print(i);
    // Serial.print("-");
    // Serial.print(t);
    // Serial.print("-");
    // Serial.print(f);
    // Serial.print("-");
    // Serial.println(val);
    
    switch(i) {
      case 0:
        crc.update(val);
        *pGuest = val;
        break;
      case 1:
        crc.update(val);
        *pHome = val;
        break;
      case 2:
        crc.update(val);
        *pTime = val;
        break;
      case 3:
        // Serial.print(val,16);
        // Serial.print("==");
        // Serial.println(crc.finalize(),16);

        if (val == crc.finalize()) {
          Serial.println();
          Serial.println("CRC ok");
          return true;
        }
        else {
          return false;
        }
        break;
    }
    i++;
    f = t+1;
  }
  return false;
}

void setup() 
{
  Serial.begin(57600);
  hc12.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  //Init and show 88-88-88
  initDisplay();
  startupDisplay();
}

void loop() 
{
  int c;

  delay(150);
  
  digitalWrite(LED_BUILTIN, HIGH);

  rxData = "";
  do {
    c = HC12Serial.read();
    if (c >= 0) {
      rxData += (char)c;
    }
  }
  while (c >= 0);
  digitalWrite(LED_BUILTIN, LOW);

  if (parseRxData(rxData, &guest, &home, &time)) {
    updateDisplay(home, guest, time);
  }
}