#include <Arduino.h>

#define DATA_EN 2

void initDisplay()
{
  Serial.begin(57600);
  pinMode(DATA_EN, OUTPUT);         //RS485 DE (Drive Enable)
  digitalWrite(DATA_EN, LOW);
}

void startupDisplay()
{
  static uint8_t sendAll[24] =
      //                                                        Gast    Meko    Time   onzichtbaar
      {'{','?','F','A','|','>','D','0','0','0','8','|','+','P','8','8','8','8','8','8','8','8','}'};
      //{'{','?','E','4','|','>','D','0','0','0','8','|','+','P','8','6','8','4','6','2','8','0','}'};
      //{'{','?','B','A','|','>','D','0','0','0','8','|','+','P','0','0','0','0','0','0','0','0','}'};

  digitalWrite(DATA_EN, HIGH);
  Serial.write((const uint8_t*)&sendAll,(size_t)23);
  Serial.flush();
  digitalWrite(DATA_EN, LOW);
}

void updateDisplay()
{
  static uint8_t selectScore[16] = 
      {'{','?','5','4','|','>','D','0','0','0','0','|','+','B','0','}'};
  static uint8_t sendScore[19] =
      {'{','?','D','B','|','>','D','0','0','0','8','|','+','P',' ','0',' ','1','}'};
  static uint8_t selectTime[16] =
      {'{','?','5','4','|','>','D','0','0','0','0','|','+','B','0','}'};
  static uint8_t sendTime[19] =
      {'{','?','F','7','|','>','D','0','0','0','4','|','+','P','2','5','-','-','}'};      // '25'
  //    {'{','?','6','6','|','>','D','0','0','0','4','|','+','P',' ',176,'0','0','}'};    // ' 0'

  digitalWrite(DATA_EN, HIGH);
  Serial.write((const uint8_t*)&selectScore,(size_t)16);
  Serial.flush();
  digitalWrite(DATA_EN, LOW);
  delay(1);
  digitalWrite(DATA_EN, HIGH);
  Serial.write((const uint8_t*)&sendScore,(size_t)19);
  Serial.flush();
  digitalWrite(DATA_EN, LOW);

  delay(1);

  digitalWrite(DATA_EN, HIGH);
  Serial.write((const uint8_t*)&selectTime,(size_t)16);
  Serial.flush();
  digitalWrite(DATA_EN, LOW);
  delay(1);
  digitalWrite(DATA_EN, HIGH);
  Serial.write((const uint8_t*)&sendTime,(size_t)19);
  Serial.flush();
  digitalWrite(DATA_EN, LOW);
}
