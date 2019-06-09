#include <Arduino.h>
#include "SoftwareSerial.h"
#include "HC12.h"

#include "../../SB-controller/include/gametime.h"
#include "../../SB-controller/include/gamescore.h"
#include "display.h"

SoftwareSerial HC12Serial(2,3);
HC12 hc12(&HC12Serial, 5, 9600);
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

  updateDisplay();

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}