#define ROW_1 2
#define ROW_2 3
#define ROW_3 4
#define ROW_4 5
#define ROW_5 6
#define ROW_6 7
#define ROW_7 8
#define ROW_8 9

#define COL_1 10
#define COL_2 11
#define COL_3 12
#define COL_4 13
#define COL_5 A0
#define COL_6 A1
#define COL_7 A2
#define COL_8 A3

const byte rows[] = {
    ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8
};
const byte col[] = {
  COL_1,COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8
};

const byte SAD[] = {

  B00000000,
  B00100100,
  B00100100,
  B00100100,
  B00000000,
  B00111100,
  B01000010,
  B01000010
};
const byte HAPPY[] = {

  B00000000,
  B00100100,
  B00100100,
  B00100100,
  B00000000,
  B01000010,
  B01000010,
  B00111100
};
const byte NEUTRAL[] = {

  B00000000,
  B00100100,
  B00100100,
  B00100100,
  B00000000,
  B01111110,
  B00000000,
  B00000000
};
const byte DED[] = {

  B00000000,
  B10100101,
  B01000010,
  B10100101,
  B00000000,
  B00111100,
  B01000010,
  B01000010
};
void setup() {
 // Open serial port
    Serial.begin(9600);
    
    // Set all used pins to OUTPUT
    // This is very important! If the pins are set to input
    // the display will be very dim.
    for (byte i = 2; i <= 13; i++)
        pinMode(i, OUTPUT);
    pinMode(A0, OUTPUT);
    pinMode(A1, OUTPUT);
    pinMode(A2, OUTPUT);
    pinMode(A3, OUTPUT);
}

void loop() {
int soilMoistureValue = analogRead(A0);

if(soilMoistureValue >= 240)
{
  drawScreen(DED);
}
else if(soilMoistureValue >= 160)
{
  drawScreen(NEUTRAL);
}
else if(soilMoistureValue > 140)
{
  drawScreen(SAD);
}
else if(soilMoistureValue > 100)
{
  drawScreen(HAPPY);
}
}
 
 void  drawScreen(byte buffer2[])
 { 
  // Turn on each row in series
    for (byte i = 0; i < 8; i++)        // count next row
     {
        digitalWrite(rows[i], HIGH);    //initiate whole row
        for (byte a = 0; a < 8; a++)    // count next row
        {
          // if You set (~buffer2[i] >> a) then You will have positive
          digitalWrite(col[a], (buffer2[i] >> a) & 0x01); // initiate whole column
          
          delayMicroseconds(100);       // uncoment deley for diferent speed of display
          //delayMicroseconds(1000);
          //delay(10);
          //delay(100);
          
          digitalWrite(col[a], 1);      // reset whole column
        }
        digitalWrite(rows[i], LOW);     // reset whole row
        // otherwise last row will intersect with next row
    }
 }

