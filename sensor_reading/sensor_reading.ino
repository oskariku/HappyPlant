#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0); 

  const int heatSensorPin(A0);
  const int soilSensorPin(A1);
  
  const int AirValue = 515;
  const int WaterValue = 255;
  int intervals = (AirValue -WaterValue)/4; // Values (wet to dry): 255-320-385-450-515
  int soilMoistureValue = 0;

 void setup(void) {
   u8g2.begin();
   Serial.begin(9600); // open serial port, baud rate 9600 bps
}
 
  float heatSensor() {
    
    // Heat sensor
    int heatValue = analogRead(heatSensorPin);
    float voltage = (heatValue/1024.0) * 5.0;   //  converts the ADC reading (analog-to-digital conversion) to voltage
    float temperature = (voltage - 0.5) * 100;   // converts the voltage to temperature in degrees
    
    return temperature;
  }
  
  int soilSensor() {

    //  Soil moisture sensor
    int soilValue;
    soilValue = analogRead(soilSensorPin); // connect sensor to Analog port-1
    return soilValue;
  }

  void loop(void) {

    Serial.print("Soil moisture value:  ");
    Serial.println(soilSensor()); // print the value to serial port 1

    Serial.print("Temperature Celcius: "); //Serial.print sends information from the Arduino to a connected computer
    Serial.println(heatSensor()); 

  // Print heat sensor and soil moisture values to OLED screen

   // HEAT VALUE
   u8g2.clearBuffer();          // clear the internal memory

    u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
    u8g2.setCursor(0,32);
    char sunsymbol[1];
    sprintf(sunsymbol, "%c", 69);
    u8g2.print(sunsymbol);
    u8g2.sendBuffer();

   u8g2.setFont(u8g2_font_ncenR14_tf);
   u8g2.setCursor(50,30);
   u8g2.print(heatSensor());
   
   u8g2.drawStr(100,30,"C");
   
   u8g2.setCursor(115,30);
   u8g2.print("\xb0");
   
   u8g2.sendBuffer();         // transfer internal memory to the display
   
   delay(2000);

   // SOIL MOISTURE VALUE

  char watersymbol[4]="";
  
  for (int i=535; i>=soilSensor(); i=i-intervals) {
    sprintf(watersymbol, "%s %c", watersymbol, 72);
  }
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_open_iconic_thing_4x_t);
  u8g2.setCursor(0,32);
  u8g2.print(watersymbol);
  u8g2.sendBuffer();

  delay(2000);

  }
    
