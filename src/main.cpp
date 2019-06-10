#include <Arduino.h>
#include "SoftwareSerial.h"
#include "HC12.h"

#include "../../SB-controller/include/gametime.h"
#include "../../SB-controller/include/gamescore.h"
#include "display.h"

SoftwareSerial HC12Serial(6,7);
HC12 hc12(&HC12Serial, 8, 9600);
GameScore gamescore;
GameTime gametime;

void setup() {
  Serial.begin(57600);
  hc12.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  initDisplay();
  startupDisplay();
  delay(2000);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);

  hc12.loop();
  updateDisplay(1,1,0);

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}