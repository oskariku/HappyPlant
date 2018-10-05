#include <EEPROM.h>

#define sampleTime 3600000

int printPin = 2;
int erasePin = 4;
int adress = 0;     //EEPROM address counter

unsigned long timer;

void clearEEPROM();
void writeValue();

void setup() {
  Serial.begin(9600);
  timer = millis();     //  millis() returns the time since the programm started
  
}

void loop() {
  if(millis()-timer > sampleTime) { // check if it is time to do a temp/moisture sensor sample 
  writeValue();
  timer = millis();
  }
  
  if(!digitalRead(printPin)) {
    //-----PrintButtonTimer = millis();
    //-----printValue(); // STATE_TEMP_AVG_D
  }
  
  if(!digitalRead(erasePin)) {
    clearEEPROM();
    delay(500);
  }
  void printValue() {
    if(NextState == STATE_TEMP || NextState == STATE_MOIST) {
      NextState = STATE_TEMP_AVG_D;
    } else if (NextState == STATE_TEMP_AVG_D) {
      NextState = STATE_TEMP_AVG_W;
    } else if (NextState == STATE_TEMP_AVG_W) {
      NextState = STATE_MOIST_AVG_D;
    } else if (NextState == STATE_MOIST_AVG_D) {
      NextState = STATE_MOIST_AVG_W;
    } else if (NextState == STATE_MOIST_AVG_W) {
      NextState = STATE_TEMP;
    }
    for(int i=0; i < EEPROM.length(); i++) {
      byte value = EEPROM.read(i);              //  read EEPROM data at address i
      if(value != 0) {
        Serial.println(temperature);
      }
    }
  }

  void clearEEPROM() {
    for(int i=0; i < EEPROM.length(); i++) {
      if (EEPROM.read(i) != 0) {
        EEPROM.write(i, 0);                 //  write 0 to address i
      }
    }
    Serial.println("Laitteen muisti tyhjennetty.")
    address = 0;
  }
  void writeValue() {
    byte value = analogRead(heatSensor);      //  Read heat sensor value
    EEPROM.write(address, value);

    Serial.print("Heat sensor value stored at address: ")
    Serial.println(address);

    adress++;
    if(address == EEMPROM.lenght()) {       //  check if address counter has reached the end of EEPROM
      address=0;                            //  if yes: reset address counter
    }
  }
}
