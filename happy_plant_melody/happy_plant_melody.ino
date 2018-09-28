// Happy plant buzzer code
// 

const int buzzer = 9; //buzzer to arduino pin 9

int tempo = 50; // ms of tempo
volatile bool soundPlaying = false;
unsigned long timeElapsed = 0;


void setup(){
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output

}

void loop(){
  if(millis()-timeElapsed > tempo) {
     playSound(100);
     timeElapsed = millis(); 
  }
  Serial.print("Toto Africa");
}

void playSound(int height) {
    if(!soundPlaying) {
      tone(buzzer, height); // Send 1KHz sound signal...
      soundPlaying=true;
      
      Serial.print("Tone:");
      Serial.println(height);
      Serial.println(timeElapsed);
    } else {
      noTone(buzzer);
      soundPlaying=false;
      
      Serial.println("Off\n");
    }
}
