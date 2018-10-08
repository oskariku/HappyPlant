//final code

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <LedControl.h> // Library for 8x8 matrix control
/*

   Main variable definitions

*/

#define sampleTime 3600000
int printPin = 2;
int erasePin = 3;
int address = 0;     //EEPROM address counter

unsigned long timer;

void clearEEPROM();
void writeValue();

unsigned long ScreenTimer;
unsigned long PrintButtonTimer;
unsigned long PlayMelodyTimer;

#define SCREEN_UPD_INTERV 2000
#define PRINT_BUTTON_INTERV 5000
#define MELODY_TRIG_INTERV 60000 //How often moisture change will be tested
#define MOISTURE_CHANGE 70 //How much moisture value must change in order to play a melody?

const int MinMoisture = 535;
const int MaxMoisture= 255;

const int MinMoistureEmoji = 500;
const int MaxMoistureEmoji = 290;

const int OptimalHighEmoji = 360;
const int OptimalLowEmoji = 430;

int interval = (MinMoisture - MinMoisture)/4; // Values (wet to dry): 255-320-385-450-515

int PrevMoisture = MaxMoisture;

volatile float temp_avg_d = 0;
volatile float moist_avg_d = 0;
volatile float temp_avg_w = 0;
volatile float moist_avg_w = 0;

//States for OLED and buttons
#define STATE_TEMP 0
#define STATE_MOIST 1

#define STATE_TEMP_AVG_D 2
#define STATE_MOIST_AVG_D 3
#define STATE_TEMP_AVG_W 4
#define STATE_MOIST_AVG_W 5
volatile byte NextState = STATE_TEMP;

/* PIN Definitions */
const int heatSensorPin(A0);
const int soilSensorPin(A1);
const int buzzer = 9;

/* PINS for 8x8 Matrix */
const int DIN_PIN = 12;
const int CLK_PIN = 11;
const int CS_PIN = 10;


/* Setupping 8x8 led matrix */
LedControl display = LedControl(DIN_PIN, CLK_PIN, CS_PIN);


/* Binary images of Led faces */
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

/* Defining OLED printing */

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

/* Defining music system */

struct Melody {
  byte notes[64]; //Array of notes
  int mlength; //Length of array
  byte tempo; // Tempo as bpm
  bool playing; //Is the melody playing?

  int gap; // gap before the next note
  float scalar; //scalar to scale length according to the tempo
  int curNote; //What note is currently playing
};

void playMelody(Melody *currentmelody, byte notes[]);
void nextNote(Melody *currentmelody, byte notes[]);


volatile Melody healing;// pokemon healing
volatile byte healing_notes[24] = {71, 8, 128, 8, 71, 8, 128, 8, 71, 8, 68, 8, 76, 2, 128, 4, 128, 2, 128, 4, 128, 2, 128, 4 };
volatile Melody totoAfrica; // Toto: Africa
volatile byte totoAfrica_notes[22] = { 61, 6, 61, 16, 0, 32, 128, 32, 61, 16, 0, 32, 128, 32, 61, 16, 61, 8, 59, 8, 64, 4  }; // Toto africa
volatile Melody tilutus;
/*volatile byte tilutus_notes[] = { 57, 64, 60, 64, 64, 64, 60, 64,
                                  57, 64, 60, 64, 64, 64, 60, 64,
                                  57, 64, 60, 64, 64, 64, 60, 64,
                                  57, 64, 60, 64, 64, 64, 60, 64,
                                  57, 128, 60, 128, 64, 128, 60, 128,
                                  57, 128, 60, 128, 64, 128, 60, 128,
                                  57, 128, 60, 128, 64, 128, 60, 128,
                                  57, 128, 60, 128, 64, 128, 60, 128
                                };

*/


void setup() {
  Serial.begin(9600);
  
  timer = millis();     //  millis() returns the time since the programm started

  ScreenTimer = millis();
  PrintButtonTimer = millis();
  PlayMelodyTimer = millis();

  //Turning 8x8 Display On
  display.clearDisplay(0);
  display.shutdown(0, false);
  display.setIntensity(0, 5);

  // Pin definitions
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

  //Start OLED
  u8g2.begin();

  cli();
  //set timer1 interrupt at 250Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 250hz increments
  //compare match register = [ 16,000,000Hz/ (prescaler * desired interrupt frequency) ] - 1
  // 250 Hz interrupt frequency (3,9 ms)
  OCR1A = 31249;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set bits for prescaler of 8
  TCCR1B |= (1 << CS11);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts


  /* Initializing music system */
  healing.mlength = 24; //length of the array
  healing.tempo = 120; // as bpm
  healing.curNote = 0;
  healing.gap = 0;
  healing.scalar = healing.tempo / 60;
  healing.playing = false;

  totoAfrica.mlength = 22;
  totoAfrica.tempo = 98;
  totoAfrica.playing = false;

  totoAfrica.curNote = 0;
  totoAfrica.gap = 0;
  totoAfrica.scalar = totoAfrica.tempo / 60;

  tilutus.mlength = 64;
  tilutus.tempo = 120;
  tilutus.playing = false;

  tilutus.curNote = 0;
  tilutus.gap = 0;
  tilutus.scalar = tilutus.tempo / 60;

  //Set midi table

}

void loop() {
  if(millis()-timer > sampleTime) { // check if it is time to do a temp/moisture sensor sample 
  writeValue();
  timer = millis();
  }
  
  if(!digitalRead(printPin)) {
    PrintButtonTimer = millis();
    //Set right state to the screen
    printValue();
    //Update OLED according to a state
    updateOled();
  }
  
  if(!digitalRead(erasePin)) {
    clearEEPROM();
    delay(500);
  }

  /* Play melody */
  if (millis() - PlayMelodyTimer > MELODY_TRIG_INTERV) {
    if(soilSensor()-PrevMoisture-MaxMoisture > MOISTURE_CHANGE) {
      healing.playing = true;
      Serial.println("Playing melody: healing.");
    }
    
    PlayMelodyTimer  = millis();
    //New value to previous moisture
    PrevMoisture = soilSensor();
  }

  /* Oled */
  if (millis() - ScreenTimer > SCREEN_UPD_INTERV) {
    updateOled();
    ScreenTimer = millis();
    Serial.print("OLED update: ");
    Serial.println(NextState);
  }

  if (millis() - PrintButtonTimer > PRINT_BUTTON_INTERV) {
    if((NextState != STATE_MOIST) && (NextState != STATE_TEMP)) {
      NextState = STATE_TEMP;
      Serial.print("Resetting OLED.");
      updateOled();
      ScreenTimer = millis();
    }
  }

  /* Led Screen */

  if (soilSensor() < MaxMoistureEmoji)
  {
    drawScreen(DED);
  }
  else if (soilSensor() >= MaxMoistureEmoji && soilSensor() < OptimalHighEmoji)
  {
    drawScreen(NEUTRAL);
  }
  else if (soilSensor() >= OptimalHighEmoji && soilSensor() < OptimalLowEmoji)
  {
    drawScreen(HAPPY);
  }
  else if (soilSensor() > OptimalLowEmoji)
  {
    drawScreen(SAD);
  }
}

/* Timer interrupt for music */
ISR(TIMER1_COMPA_vect) {

  if (healing.playing) {
    playMelody(&healing, healing_notes);
  }
}

float heatSensor() {

  // Heat sensor
  int heatValue = analogRead(heatSensorPin);
  float voltage = (heatValue / 1024.0) * 5.0; //  converts the ADC reading (analog-to-digital conversion) to voltage
  float temperature = (voltage - 0.5) * 100;   // converts the voltage to temperature in degrees

  return temperature;
  Serial.print(heatSensor());
}
int soilSensor() {

  //  Soil moisture sensor
  int soilValue;
  soilValue = analogRead(soilSensorPin); // connect sensor to Analog port-1  
  return soilValue;
  Serial.print(soilSensor());
}

void printValue() {
  switch(NextState) {
    case STATE_TEMP:
    case STATE_MOIST:
      NextState = STATE_TEMP_AVG_D;
      break;
    case STATE_TEMP_AVG_D:
      NextState = STATE_TEMP_AVG_W;
      break;
    case STATE_TEMP_AVG_W:
      NextState = STATE_MOIST_AVG_D;
      break;
    case STATE_MOIST_AVG_D:
      NextState = STATE_MOIST_AVG_W;
      break;
    case STATE_MOIST_AVG_W:
      NextState = STATE_TEMP;
      break;
  }

  /*
   * Not sure what this code is for...
    for(int i=0; i < EEPROM.length(); i++) {
    byte value = EEPROM.read(i);              //  read EEPROM data at address i
    if(value != 0) {
      Serial.println(heatSensor());
    }
  }*/
}

void clearEEPROM() {
  for(int i=0; i < EEPROM.length(); i++) {
    if (EEPROM.read(i) != 0) {
      EEPROM.write(i, 0);                 //  write 0 to address i
    }
  }
  Serial.println("Laitteen muisti tyhjennetty.");
  address = 0;
}

void writeValue() {
  int heat = heatSensor();
  int moist = soilSensor() / 2.2; // We must scale value to fit it between 0-255
  byte heatValue = (byte) heat;
  byte moistValue = (byte) moist;
  EEPROM.write(address, heatValue);
  EEPROM.write(address+1, moistValue);

  Serial.print("Heat sensor value stored at address: ");
  Serial.println(address);
  Serial.print("Moisture sensor value stored at address: ");
  Serial.println(address+1);

  address+=2;
  if(address == EEPROM.length()) {       //  check if address counter has reached the end of EEPROM
    address=0;                            //  if yes: reset address counter
  }
}

void countAverages() {

  temp_avg_d = 0;
  moist_avg_d = 0;
  temp_avg_w = 0;
  moist_avg_w = 0;

  //How many samples are saved during a day/week
  int sample_amount_d = 86400000 / sampleTime * 2;
  int sample_amount_w = 604800000 / sampleTime * 2; // There are 604800000 milliseconds in a week.

  int samples = address - sample_amount_w;
  if(samples < 0) {
    samples = 0;
  }

  Serial.print("Counting averages. Sample amount D / W :");
  Serial.print(sample_amount_d, sample_amount_w);

  for(int i = address-1; i > samples; i-=2) {
    byte value = EEPROM.read(i);
    int temperature = (int) value;
    value = EEPROM.read(i+1);
    int moisture = value * 3;
    if (i > address-sample_amount_d) {
      temp_avg_d += temperature;
      moist_avg_d += moisture;
    }
    temp_avg_w += temperature;
    moist_avg_w += moisture;
  }

  temp_avg_d = temp_avg_d / (sample_amount_d/2);
  moist_avg_d = moist_avg_d /(sample_amount_d/2);
  temp_avg_w = temp_avg_w / (sample_amount_w/2);
  moist_avg_w = moist_avg_w /(sample_amount_w/2);
}

void updateOled() {
  // Print heat sensor and soil moisture values to OLED screen

  u8g2.clearBuffer();          // clear the internal memory

  u8g2.setFont(u8g2_font_ncenR14_tf);
  u8g2.setCursor(0, 0);
  char text[20] ="";

  
  if (NextState == STATE_TEMP_AVG_D) {
    // Print daily average temperature
    Serial.print("Temp avg d: ");
    Serial.println(temp_avg_d);
    sprintf(text, "Daily avg temp: %d", temp_avg_d);

  } else if (NextState == STATE_MOIST_AVG_D) {
    //Print daily average moisture
    Serial.print("Moist avg d: ");
    Serial.println(moist_avg_d);
    sprintf(text, "Daily avg moist: %d", moist_avg_d);

  } else if (NextState == STATE_TEMP_AVG_W) {
    //Print weekly average temperature
    Serial.print("Temp avg w: ");
    Serial.println(temp_avg_w);
    sprintf(text, "Weekly avg temp: %d", temp_avg_w);

  } else if (NextState == STATE_MOIST_AVG_W) {
    //Print weekly average moisture
    Serial.print("Moist avg w: ");
    Serial.println(moist_avg_w);
    sprintf(text, "Weekly avg moist: %d", moist_avg_d);

  } else if (NextState == STATE_TEMP) {
    // Print temperature

    u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
    u8g2.setCursor(0, 32);
    char sunsymbol[1];
    sprintf(sunsymbol, "%c", 69);
    u8g2.print(sunsymbol);

    u8g2.setFont(u8g2_font_ncenR14_tf);
    u8g2.setCursor(50, 30);
    u8g2.print(heatSensor());

    u8g2.drawStr(100, 30, "C");

    u8g2.setCursor(115, 30);
    u8g2.print("\xb0");

    NextState = STATE_MOIST;

  } else if (NextState == STATE_MOIST) {
    //Print moisture
  
    char watersymbol[4]="";
    
    for(int i=MinMoisture; i>=soilSensor(); i=i-interval) {
    sprintf(watersymbol, "%s %c", watersymbol, 72);
    }
    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_open_iconic_thing_4x_t);
    u8g2.setCursor(0, 32);
    u8g2.print(watersymbol);
    
    NextState = STATE_TEMP;
  }

  //Print text if any:
  u8g2.print(text);
  u8g2.sendBuffer();         // transfer internal memory to the display
}

// function that draws smileys
void  drawScreen(byte buffer2[])
{
  // Turn on each row in series
  for (byte i = 0; i < 8; i++)        // count next row
  {
    for (byte a = 0; a < 8; a++)    // count next row
    {
      display.setLed(0, i, a, bitRead(buffer2[i], 7 - a));
      /* Debug outputs
        Serial.print(" col ");
        Serial.print(i);
        Serial.print(a);
      */
    }
    //Serial.println();

  }
}

/* Music system related functions begin here */
void playMelody(Melody *currentmelody, byte notes[]) {
  if (currentmelody->gap <= 0) {
    nextNote(currentmelody, notes);
  }
  currentmelody->gap--;

}

void nextNote(Melody *currentmelody, byte notes[]) {
  int height = midi(notes[currentmelody->curNote]);
  int noteLength = notes[currentmelody->curNote + 1];
  int note = notes[currentmelody->curNote];

  noTone(buzzer);

  //note 128 is rest
  if (note < 128) {
    tone(buzzer, height);
  }

  currentmelody->gap = 256 / noteLength / currentmelody->scalar;

  currentmelody->curNote += 2;
  if (currentmelody->curNote > currentmelody->mlength) {
    currentmelody->curNote = 0;
    currentmelody->playing = false;
    noTone(buzzer);
  }
}

/* Helper function for music system */

int midi(byte note) {
  //Returns midi note by frequency
  int midi[127];
  midi[0] = 8;
  midi[1] = 8;
  midi[2] = 9;
  midi[3] = 9;
  midi[4] = 10;
  midi[5] = 10;
  midi[6] = 11;
  midi[7] = 12;
  midi[8] = 12;
  midi[9] = 13;
  midi[10] = 14;
  midi[11] = 15;
  midi[12] = 16;
  midi[13] = 17;
  midi[14] = 18;
  midi[15] = 19;
  midi[16] = 20;
  midi[17] = 21;
  midi[18] = 23;
  midi[19] = 24;
  midi[20] = 25;
  midi[21] = 27;
  midi[22] = 29;
  midi[23] = 30;
  midi[24] = 32;
  midi[25] = 34;
  midi[26] = 36;
  midi[27] = 38;
  midi[28] = 41;
  midi[29] = 43;
  midi[30] = 46;
  midi[31] = 48;
  midi[32] = 51;
  midi[33] = 55;
  midi[34] = 58;
  midi[35] = 61;
  midi[36] = 65;
  midi[37] = 69;
  midi[38] = 73;
  midi[39] = 77;
  midi[40] = 82;
  midi[41] = 87;
  midi[42] = 92;
  midi[43] = 97;
  midi[44] = 103;
  midi[45] = 110;
  midi[46] = 116;
  midi[47] = 123;
  midi[48] = 130;
  midi[49] = 138;
  midi[50] = 146;
  midi[51] = 155;
  midi[52] = 164;
  midi[53] = 174;
  midi[54] = 184;
  midi[55] = 195;
  midi[56] = 207;
  midi[57] = 220;
  midi[58] = 233;
  midi[59] = 246;
  midi[60] = 261;
  midi[61] = 277;
  midi[62] = 293;
  midi[63] = 311;
  midi[64] = 329;
  midi[65] = 349;
  midi[66] = 369;
  midi[67] = 391;
  midi[68] = 415;
  midi[69] = 440;
  midi[70] = 466;
  midi[71] = 493;
  midi[72] = 523;
  midi[73] = 554;
  midi[74] = 587;
  midi[75] = 622;
  midi[76] = 659;
  midi[77] = 698;
  midi[78] = 739;
  midi[79] = 783;
  midi[80] = 830;
  midi[81] = 880;
  midi[82] = 932;
  midi[83] = 987;
  midi[84] = 1046;
  midi[85] = 1108;
  midi[86] = 1174;
  midi[87] = 1244;
  midi[88] = 1318;
  midi[89] = 1396;
  midi[90] = 1479;
  midi[91] = 1567;
  midi[92] = 1661;
  midi[93] = 1760;
  midi[94] = 1864;
  midi[95] = 1975;
  midi[96] = 2093;
  midi[97] = 2217;
  midi[98] = 2349;
  midi[99] = 2489;
  midi[100] = 2637;
  midi[101] = 2793;
  midi[102] = 2959;
  midi[103] = 3135;
  midi[104] = 3322;
  midi[105] = 3520;
  midi[106] = 3729;
  midi[107] = 3951;
  midi[108] = 4186;
  midi[109] = 4434;
  midi[110] = 4698;
  midi[111] = 4978;
  midi[112] = 5274;
  midi[113] = 5587;
  midi[114] = 5919;
  midi[115] = 6271;
  midi[116] = 6644;
  midi[117] = 7040;
  midi[118] = 7458;
  midi[119] = 7902;
  midi[120] = 8372;
  midi[121] = 8869;
  midi[122] = 9397;
  midi[123] = 9956;
  midi[124] = 10548;
  midi[125] = 11175;
  midi[126] = 11839;
  midi[127] = 12543;
  return midi[note];
}
