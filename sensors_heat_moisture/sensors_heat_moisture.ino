

void setup() {
  Serial.begin(9600); // open serial port, baud rate 9600 bps
}
  
  const int heatSensorPin(A0);
  const float baselineTemp = 23.75;

  const int soilSensorPin(A1);
  const int AirValue = 515;
  const int WaterValue = 255;
  int intervals = (AirValue -WaterValue)/4; // Values (wet to dry): 65-130-195-260
  int soilMoistureValue = 0;
 
  float heatSensor() {
    
    // Heat sensor
    int heatValue = analogRead(heatSensorPin);
    float voltage = (heatValue/1024.0) * 5.0;   //  converts the ADC reading (analog-to-digital conversion) to voltage
    float temperature = (voltage - 0.5) * 100;   // convert the voltage to temperature in degrees
    
    return temperature;
  }
  
  int soilSensor() {

    int soilValue;
    soilValue = analogRead(soilSensorPin); // connect sensor to Analog port-1
    return soilValue;
  }

  void loop() {
    Serial.print("Soil moisture value:  ");
    Serial.println(soilSensor()); // print the value to serial port 1

    Serial.print("Temperature Celcius: "); //Serial.print sends information from the Arduino to a connected computer
    Serial.println(heatSensor());
    
    delay(5000);    
    }
