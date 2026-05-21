
// ------------------------------------------------------------
//  Task 2 and 3:
//      Write the body of setTimer1Freq() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER1_IRQHandler().
//      Make necessary changes for setBuzzerFreq()
// ------------------------------------------------------------

#include <Arduino.h>

bool buzzerHigh = false;

void setup() {
  NRF_P0->DIRSET = (1UL << 29); // Set P0.29 as output
  setTimer1Freq();
}


void loop() {

}


/*
void setTimer1Freq() {
  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->TASKS_CLEAR = 1;
  NRF_TIMER1->MODE = 0;
  NRF_TIMER1->BITMODE = 0;
  NRF_TIMER1->PRESCALER = 4;
  NRF_TIMER1->CC[0] = 478;
  NRF_TIMER1->SHORTS = (1UL << 0);
  NRF_TIMER1->INTENSET = (1UL << 16);
  NVIC_EnableIRQ(TIMER1_IRQn);
  NRF_TIMER1->TASKS_START = 1;
}
*/


void setBuzzerFreq() {

}


extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]){
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
  }
  if (buzzerHigh){
    NRF_P0->OUTCLR = (1UL << 29);
      buzzerHigh = false;
  }
  else {
    NRF_P0->OUTSET = (1UL << 29);
    buzzerHigh = true;
  }
}



