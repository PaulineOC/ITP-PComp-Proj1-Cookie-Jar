#include "pitches.h"

//Analog
int FSRPin = A0;

//Digital
int switchPin = 7;
int LEDPin = 8;
int speakerPin = 9;

// extra safety net boolean to make sure the speaker doesn't play incorrectly and doesn't break with noTone()
boolean speakerIsPlaying; 

//Timer:
unsigned long startMillis;
unsigned long currentMillis;
int openCount = 0;
int thresholdCount = 2;
int period = 10000; // in milliseconds --> 10 seconds

//Impending doom tune
int melody[] = {
NOTE_A3, NOTE_A3,NOTE_A3, NOTE_F3, NOTE_C4, NOTE_A3, NOTE_F3, NOTE_C4, NOTE_A3, NOTE_E4,NOTE_E4,NOTE_E4,NOTE_F4,NOTE_C4, NOTE_GS3,NOTE_F3,NOTE_C4,NOTE_A3,
};

int noteDurations[] = {4,4,4,8,8,4,8,8,2,4,4,4,8,8,4,8,8,2};

// Stages
int currState;
int prevState = LOW;
int prevWeight;
int currWeight;
int weightThreshold = 18;

void setup() {
  Serial.begin(9600);
  
  pinMode(switchPin, INPUT);
  pinMode(LEDPin, OUTPUT);

  speakerIsPlaying = false;
  startMillis = -1;  //initial start time
}

void loop() {
    delay(500);
    currentMillis = millis();
    currState = digitalRead(switchPin);
    currWeight = analogRead(FSRPin);
    
    Serial.println(analogRead(FSRPin));
    //Serial.println(digitalRead(switchPin));

    // If outside the interval, reset the open count
    if(currentMillis - startMillis > period){
       openCount = 0;
    }

    // Open after previously closed
    if(currState == LOW){
      digitalWrite(LEDPin, HIGH);

       Serial.print("open ");
       Serial.println(openCount);
       
      // Weight has decreased
      if(prevWeight - currWeight > weightThreshold){
        openCount++;
        startMillis = openCount > 1 ? startMillis : currentMillis;
        if(openCount > thresholdCount){ // If jar opened more than accepted # of times 
          playTune();
        }
      }
    }
    else {
      digitalWrite(LEDPin, LOW);speakerIsPlaying = false;
      noTone(speakerPin);
    }
    prevWeight = currWeight;
    prevState = currState;
}

void playTune(){
  speakerIsPlaying = true;
  for (int thisNote = 0; thisNote < 18; thisNote++) {
    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(speakerPin, melody[thisNote],noteDuration);
    //pause for the note's duration plus 30 ms:
    delay(noteDuration +50);
  }
}
