
// ------------------------------------------------------------
//  Task 4:
//      Make necessary changes to your code from Task 3. Provided tests
//      do not cover all cases.
// ------------------------------------------------------------

#include <Arduino.h>
volatile bool buzzerHigh = false;
volatile bool buzzerEnabled = false;

void setup() {
  NRF_P0->DIRSET = (1UL << 29); // Set P0.29 as output
  NRF_TIMER1->MODE = 0;
  NRF_TIMER1->BITMODE = 3;
  NRF_TIMER1->PRESCALER = 4;
  NRF_TIMER1->SHORTS = (1UL << 0);
  NRF_TIMER1->INTENSET = (1UL << 16);
  NVIC_EnableIRQ(TIMER1_IRQn);
}


void loop() {
// tests
  setBuzzerFreq(440);  //A4
  delay(300);
  setBuzzerFreq(554);  //C#5
  delay(300);
  setBuzzerFreq(659);  //E5
  delay(300);
  setBuzzerFreq(880);  //A5
  delay(500);
  setBuzzerFreq(5000); //out of range
  delay(3000);
  for (int f = 100; f <= 3000; f += 50) {
    setBuzzerFreq(f);
    delay(20);}
}


void setBuzzerFreq(uint32_t freq) {
  if (freq < 100 || freq > 3000){
    NRF_TIMER1->TASKS_STOP = 1;
    NRF_P0->OUTCLR = (1UL << 29);
    buzzerHigh = false;
    buzzerEnabled = false;
    return;
  }
  NRF_TIMER1->CC[0] = 1000000/(2 * freq);

  NRF_TIMER1->TASKS_CLEAR = 1;
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