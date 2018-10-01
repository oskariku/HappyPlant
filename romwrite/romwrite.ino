#include <EEPROM.h>

int address = 0;
byte value;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 while(!Serial) {
  ; //wait until serial is up and running
 }
 for( int i = 0; i < 10; i++) {
  EEPROM.write(i, 255-i*2);
  delay(100);
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
