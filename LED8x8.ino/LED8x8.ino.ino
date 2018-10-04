#include <LedControl.h>

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

const int DIN_PIN = 12;
const int CLK_PIN = 11;
const int CS_PIN = 10;

LedControl display = LedControl(DIN_PIN, CLK_PIN, CS_PIN);

void setup() {
  // Open serial port
  Serial.begin(9600);
  display.clearDisplay(0);
  display.shutdown(0, false);
  display.setIntensity(0,5);
}

void loop() {
  while (Serial.available() > 0) {
    int soilMoistureValue = Serial.parseInt();
    Serial.print("Reading values ");
    Serial.print(soilMoistureValue);
    
        Serial.println("out of loop");
      if (soilMoistureValue >= 240)
      {
        drawScreen(DED);
        Serial.print("DED");
      }
      else if (soilMoistureValue >= 160)
      {
        drawScreen(NEUTRAL);
        Serial.print("NEUTRAL");
      }
      else if (soilMoistureValue > 140)
      {
        drawScreen(SAD);
        Serial.print("SAD");
      }
      else if (soilMoistureValue > 100)
      {
        drawScreen(HAPPY);
        Serial.print("HAPPY");
      }
    }
  }


void  drawScreen(byte buffer2[])
{
  // Turn on each row in series
  for (byte i = 0; i < 8; i++)        // count next row
  {
    for (byte a = 0; a < 8; a++)    // count next row
    {
      display.setLed(0, i, a, bitRead(buffer2[i], 7 - a));
      Serial.print(" col ");
      Serial.print(i);
      Serial.print(a);
    }
    Serial.println();

  }
}

