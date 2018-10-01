// Happy plant buzzer code
// 

const int buzzer = 9; //buzzer to arduino pin 9

volatile int midi[127];

byte tempo = 120; // bpm
volatile byte melody[] = {  71, 8, 128, 8, 71, 8, 128, 8, 71, 8, 68, 8, 76, 2, 128, 4, 128, 2, 128, 4, 128, 2, 128, 4 }; // pokemon healing
volatile int songLength = 24; // length of the array

volatile int currentNote = 0; //which note of the melody is playing
volatile float scalar = tempo / 60; //Scalar to music algorithm
volatile int gap = 0; // Gap between note requests
volatile bool startPlaying = true;

void setup(){
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

  cli();
  //set timer1 interrupt at 250Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 250hz increments
  //compare match register = [ 16,000,000Hz/ (prescaler * desired interrupt frequency) ] - 1
  // 64 Hz interrupt frequency (15,5 ms)
  OCR1A = 31249;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set bits for prescaler of 8
  TCCR1B |= (1 << CS11);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts
  
  //Set midi table

  
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
  
}

void loop(){
  Serial.print("test test");
}

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)

  if(startPlaying) {
    playMelody(melody);
  }
}

void playMelody(byte melody[]) {
  if(gap <= 0) {
    nextNote(melody);
  }
  gap = gap-1;

}

void nextNote(byte melody[]) {
  int height = midi[melody[currentNote]];
  int noteLength = melody[currentNote + 1];
  int note = melody[currentNote];

  noTone(buzzer);
  
  //note 128 is rest
  if(note < 128) {
    tone(buzzer, height);
  }

  gap = 256 / noteLength / scalar;
  
  currentNote += 2;
  if(currentNote > songLength) {
    currentNote = 0;
    startPlaying = false;
    noTone(buzzer);
  }
}
