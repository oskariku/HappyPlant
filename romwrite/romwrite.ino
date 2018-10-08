#include <EEPROM.h>

int address = 0;
byte value;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 while(!Serial) {
  ; //wait until serial is up and running
 }
 for( int i = 0; i < 336; i+=2) {
  EEPROM.write(i, 128);
  delay(100);
 }
 for( int i = 1; i < 336; i+=2) {
  EEPROM.write(i, 22);
 }

}

void loop() {
  
  value = EEPROM.read(address);

  Serial.print(address);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();

  address++;
  if(address == EEPROM.length()) {
    address = 0;
  }

  delay(500);

}
