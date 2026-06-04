
// ------------------------------------------------------------
//  Task 6:
//      Use your code from Task 4 and Task 5, and make necessary changes. 
//      Implement the function playMelody() according to the exercise sheet.
//      The array is already filled with the right frequency for each note.
// ------------------------------------------------------------

#include <Arduino.h>
uint16_t durations[10] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};
uint16_t notes[10]     = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659};
volatile uint32_t tCount = 0;
volatile uint8_t melodyIdx = 0;

volatile bool buzzerHigh = false;


void setup() {
  NRF_P0->DIRSET = (1UL << 29); // Set P0.29 as output

  playMelody();
}


void loop() {

}


void playMelody() {
  setTimer2(true);
  setBuzzerFreq(notes[0]);

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

// fuctions from previous exercises
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