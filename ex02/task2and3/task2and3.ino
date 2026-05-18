// ------------------------------------------------------------
//  Task 2 and 3:
//      Write the body of setTimer1Freq() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER1_IRQHandler().
//      Make necessary changes for setBuzzerFreq()
// ------------------------------------------------------------

#include <Arduino.h>


void setup() {
  setTimer1Freq();
}


void loop() {

}



void setTimer1Freq() {
  NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
  NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_32bit;
  NRF_TIMER->PRESCALER;
}



void setBuzzerFreq() {

}


extern "C" void TIMER1_IRQHandler() {

}



