
// ------------------------------------------------------------
//  Task 7:
//      Use your code from Task 6, adding a parser function to
//      go over any given string and create the array needed to play 
//      it.
//      You are free to use or discard any helper functions and add
//      any helper functions you need for parsing.
// ------------------------------------------------------------

#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
const char noteNames[] = {'c', 'C', 'd', 'D', 'e', 'f', 'F', 'g', 'G', 'a', 'A', 'b'};
const uint16_t notes[] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494};
char buffer[]="Test:d=4,o=5,b=200:8g,8a,8c6,8a,e6,8p,e6,8p,d6.,8p,8g,8a,8c6,8a,d6,8p,d6,8p,c6,8p,a.,8g,8a,8c6,8a,2c6,d6,b,a,g.,8p,g,2d6,2c6.,p,8g,8a,8c6,8a,e6,8p,e6,8p,d6.,8p,8g,8a,8c6,8a,g6,b,c6,8p,b,8a,p,8g,8a,8c6,8a,2c6,d6,b,a,g,p,g,d6,c6"
volatile uint8_t melodyIdx = 0;
volatile uint32_t tCount = 0;
uint16_t standardDuration = 4;
uint16_t standardOctave = 6;
uint16_t standardBPM = 63;
bool raiseOctave = false;


void setup() {
  String song0 = "";
  String song1 = "GoodSong1:d=4,o=4,b=112:c,d#,f.,c,d#,8f#,f,p,c,d#,f.,d#,c";
  String song2 = "GoodSong2:o=5,d=4,b=320,b=320:c,8d,8d,d,2d,c,c,c,c,8d#,8d#,2d#,d,d,d,c,8d,8d,d,2d,c,c,c,c,8d#,8d#,d#,2d#,d,c#,c,c6,1b.,g,f,1g.";
  String song3 = "GoodSong3:o=5,d=8,b=112,b=112:d,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,d,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,c,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,c,d,a,d,e6,d,d6,d,a,d,e6,d,d6";
  String song4 = "GoodSong4:o=5,d=8,b=125,b=125:16g,16g,a#.,16g,16p,16g,c6,g,f,4g,d6.,16g,16p,16g,d#6,d6,a#,g,d6,g6,16g,16f,16p,16f,d,a#,2g,4p,16f6,d6,c6,a#,4g,a#.,16g,16p,16g,c6,g,f,4g,d6.,16g,16p,16g,d#6,d6,a#,g,d6,g6,16g,16f,16p,16f,d,a#,2g";
  String song5 = "GoodSong5:o=5,d=16,b=100,b=100:g,g,a,a,e,e,8g,g,g,a,a,e,e,8g,g,g,a,a,c6,c6,8b,8b,8a,8g,8f,f,f,g,g,d,d,8f,f,f,g,g,d,d,8f,f,f,g,g,a,b,8c6,8a,8g,8e,4c";
  String song6 = "GoodSong6:o=5,d=8,b=140,b=140:g,e,4p,p,e,f,g,e6,p,e6,p,2c6,g,e,4p,p,e,f,e,g,p,g,p,2f,f,d,4p,p,d,e,f,g,e,4p,p,e,f#,e,d,g,p,e,f#,d,p,a,g.,16f#,g,a,g,f,e,d";
  String song7 = "GoodSong7:o=5,d=8,b=63,b=63:a4,c,e,a,b,e,c,b,c6,e,c,c6,f#,d,a4,f#,e.,16c,a4,4e,c,a4,e,g4,a4,4a4";
  String song8 = "GoodSong8:o=5,d=8,b=200,b=200:g#,4c#,p,4c#6,a#,4g#,4c#,p,4g#,f#,f,f,f#,g#,4c#,4d#,2f,2p,4g#,4c#,p,4c#6,a#,4g#,4c#,p,4g#,f#,f,f,f#,g#,4c#,4d#,2c#";
  String song9 = "GoodSong9:o=4,d=8,b=125,b=125:c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6,2p,c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6";
  String song10 = "GoodSong10:o=5,d=8,b=160,b=160:c#6,a#,2p,a#,g#,f#,g#,a#,4c#6,a#,4c#6,d#6,a#,2p,a#,g#,f#,g#,a#,4c#6,a#,4c#6,d#6,b,2p,b,a#,g#,a#,b,4d#6,f#6,4d#6,4f6.,4d#6.,4c#6.,4b.,4a#,4g#";
  String song11 = "GoodSong11:o=5,d=16,b=125,b=125:b,a,4b,4e,4p,8p,c6,b,8c6,8b,4a,4p,8p,c6,b,4c6,4e,4p,8p,a,g,8a,8g,8f#,8a,4g.,f#,g,4a.,g,a,8b,8a,8g,8f#,4e,4c6,2b.,b,c6,b,a,1b";
  playRTTTL();
}


void loop() {

}


void playRTTTL() {

}


bool parseRTTLNote(Note * note) {

}


uint16_t freqFromNote(char note, bool sharp) {

  uint16_t provisionalNote;


  // length of notes/ notenames is 12
  for (int i = 0; i < 12; i++){
    if( noteNames[i] == note){

      // raise by semitone
      if(sharp){
        if (i < 11){
          return notes[i+1];
        }
        else{
          raiseOcatve = true;
          return notes[0];
        }
      }

      return notes[i];
    }
  }
  // if we dont know the note; 0 is outside of the range for setBuzzzer freq
  return 0;
}


uint16_t str2uint(char * buf, uint16_t * idx) {
  uint16_t val = 0;
  while(char isDigit)
}


bool isDigit(char c) { 
  return c >= 0 && c<= 9;
}

// from here on is the code from Task 6
void setBuzzerFreq(uint32_t freq) {
  if (freq < 100 || freq > 3000){
    NRF_TIMER1->TASKS_STOP = 1;
    NRF_P0->OUTCLR = (1UL << 29);
    buzzerHigh = false;
    return;
  }
  NRF_TIMER1->CC[0] = 1000000/(2 * freq);

  NRF_TIMER1->TASKS_CLEAR = 1;

  NRF_TIMER1->MODE = 0;
  NRF_TIMER1->BITMODE = 3;
  NRF_TIMER1->PRESCALER = 4;
  NRF_TIMER1->SHORTS = (1UL << 0);
  NRF_TIMER1->INTENSET = (1UL << 16);
  NVIC_EnableIRQ(TIMER1_IRQn);
  NRF_TIMER1->TASKS_START = 1;

}


extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]){
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    if (buzzerHigh){
    NRF_P0->OUTCLR = (1UL << 29);
      buzzerHigh = false;
    }
    else {
      NRF_P0->OUTSET = (1UL << 29);
      buzzerHigh = true;
    }
  }
}


void setTimer2(bool enable) {
  if(enable){
  NRF_TIMER2->TASKS_STOP = 1;
  NRF_TIMER2->TASKS_CLEAR = 1;
  NRF_TIMER2->MODE = 0;
  NRF_TIMER2->BITMODE = 3;
  NRF_TIMER2->PRESCALER = 4;
  NRF_TIMER2->CC[0] = 1000; 
  NRF_TIMER2->SHORTS = (1UL << 0);
  NRF_TIMER2->INTENSET = (1UL << 16); 
  NVIC_EnableIRQ(TIMER2_IRQn);
  NRF_TIMER2->TASKS_START = 1;
  }
  else{
    NRF_TIMER2->TASKS_STOP = 1;
    NVIC_DisableIRQ(TIMER2_IRQn);
  }
}

extern "C" void TIMER2_IRQHandler() {
  if (NRF_TIMER2->EVENTS_COMPARE[0]){
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;
    tCount++;
    if(tCount >= durations[melodyIdx]){
      tCount = 0;
      melodyIdx++;
      if(melodyIdx >= 10){
        setTimer2(false);
        setBuzzerFreq(0); // out of range, silences buzzer
      }
      else{
        setBuzzerFreq(notes[melodyIdx]);
      }
    }
  }
}
