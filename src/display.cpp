#include <Arduino.h>

#define DATA_EN 2

#define CHECKSUM_FIRST_DIG  2
#define SCORE_FIRST_DIG     14
#define TIME_FIRST_DIG      14
#define TIME_ZERO           176
#define FIRSTNONSPACE       0x10

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

void updateDisplay(int home, int guest, int time)
{
    uint8_t checksum=0, timeMin, timeSec, showTime;
    char hex[17]="0123456789ABCDEF";

    /*************************************************************************
    Score messages
    {	?	D	B	|	>	D	0	0	0	8	|	+	P	' '	0	' '	1	}
    {	?	E	3	|	>	D	0	0	0	8	|	+	P	' '	0	' '	9	}
    {	?	E	C	|	>	D	0	0	0	8	|	+	P	' '	0	1	1	}
    {	?	F	5	|	>	D	0	0	0	8	|	+	P	' '	1	1	9	}
    {	?	0	5	|	>	D	0	0	0	8	|	+	P	1	0	1	9	}
    {	?	F	C	|	>	D	0	0	0	8	|	+	P	1	0	1	0	}
    */

    static uint8_t selectScore[16] = 
        {'{','?','5','4','|','>','D','0','0','0','0','|','+','B','0','}'};
    static uint8_t sendScore[19] =                              // Gasten - Thuis
        //{'{','?','D','A','|','>','D','0','0','0','8','|','+','P',' ','0',' ','0','}'};
        //{'{','?','E','3','|','>','D','0','0','0','8','|','+','P',' ','0',' ','9','}'};
        //{'{','?','E','B','|','>','D','0','0','0','8','|','+','P',' ','0','1','0','}'};
        {'{','?','E','B','|','>','D','0','0','0','8','|','+','P','1','0',' ','0','}'};
        //{'{','?','F','C','|','>','D','0','0','0','8','|','+','P','1','0','1','0','}'};

    checksum = 0xDA;
    if (home < 10) {
        sendScore[SCORE_FIRST_DIG+0] = ' ';
        sendScore[SCORE_FIRST_DIG+1] = '0' + home;
        checksum += home;
    }
    else if (home < 100) {
        checksum += FIRSTNONSPACE;
        sendScore[SCORE_FIRST_DIG+0] = '0' + (home/10);
        sendScore[SCORE_FIRST_DIG+1] = '0' + (home%10);
        checksum += (home%10);
        checksum += (home/10);
    }

    if (guest < 10) {
        sendScore[SCORE_FIRST_DIG+2] = ' ';
        sendScore[SCORE_FIRST_DIG+3] = '0' + guest;
        checksum += guest;
    } 
    else if (guest < 100) {
        checksum += FIRSTNONSPACE;
        sendScore[SCORE_FIRST_DIG+2] = '0' + (guest/10);
        sendScore[SCORE_FIRST_DIG+3] = '0' + (guest%10);
        checksum += (guest%10);
        checksum += (guest/10);
    }
    sendScore[CHECKSUM_FIRST_DIG+0] = hex[ (checksum>>4) & 0x0F ];
    sendScore[CHECKSUM_FIRST_DIG+1] = hex[ checksum & 0x0F ];

    digitalWrite(DATA_EN, HIGH);
    Serial.write((const uint8_t*)&selectScore,(size_t)16);
    Serial.flush();
    digitalWrite(DATA_EN, LOW);
    delay(1);
    digitalWrite(DATA_EN, HIGH);
    Serial.write((const uint8_t*)&sendScore,(size_t)19);
    Serial.flush();
    digitalWrite(DATA_EN, LOW);


    /*************************************************************************
    Time messages
    {	?	5	4	|	>	D	0	0	0	0	|	+	B	0	}
    {	?	F	7	|	>	D	0	0	0	4	|	+	P	1	5	-	-	}
    {	?	F	7	|	>	D	0	0	0	4	|	+	P	2	5	-	-	}
    {	?	6	6	|	>	D	0	0	0	4	|	+	P	' '	176	0	0	}   //0'00"
    {	?	6	E	|	>	D	0	0	0	4	|	+	P	' '	176	5	3	}   //0'53"
    {	?	7	A	|	>	D	0	0	0	4	|	+	P	1	176	0	3	}   //10'03"

    Seconds control does not seem to have much logic in it.
       - 4 digits are uses, only top 2 are shown. We keep the bottom half fixed at 0.
       - Checksum for top digit needs +0x10 in checksum
       - Lower digit has wierd offset; 0 = 176 ... 9 = 185

    This works, leave it alone or tread with caution !!!
    */

    static uint8_t selectTime[16] =
        {'{','?','5','4','|','>','D','0','0','0','0','|','+','B','0','}'};
    static uint8_t sendTime[19] =
        //{'{','?','F','7','|','>','D','0','0','0','4','|','+','P','2','5','-','-','}'};      // '25'
        {'{','?','6','6','|','>','D','0','0','0','4','|','+','P',' ',176,'0','0','}'};    // ' 0'

    checksum = 0x66;
    timeMin = time/60;
    timeSec = time%60;

    // Show minutes normally, move to seconds on the last minute
    if (timeMin > 0) {
        showTime = timeMin;
    } else {
        showTime = timeSec;
    }
    sendTime[TIME_FIRST_DIG+0] = ((showTime/10) == 0 ? ' ' : (showTime/10) + '0');
    sendTime[TIME_FIRST_DIG+1] = ((showTime%10) == 0 ? TIME_ZERO : (showTime%10) + TIME_ZERO);
    checksum += ((showTime/10) == 0 ? 0 : ((showTime/10) + 0x10));
    checksum += (showTime%10);

    sendTime[CHECKSUM_FIRST_DIG+0] = hex[ (checksum>>4) & 0x0F ];
    sendTime[CHECKSUM_FIRST_DIG+1] = hex[ checksum & 0x0F ];

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
