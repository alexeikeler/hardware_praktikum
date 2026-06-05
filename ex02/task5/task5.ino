
// ------------------------------------------------------------
//  Task 5:
//      Write the body of setTimer2() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER2_IRQHandler().
// ------------------------------------------------------------

#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
volatile uint32_t tCount = 0;
// helper to be able to print after a set interval
uint32_t printcount = 0;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Initializing Timer 2 (1ms interval)...");
  setTimer2(true);
}


void loop() {
  // tcount/ printcount ar in ms -> print every second
  if(tCount -printcount >= 1000){
    printcount= tCount;
    Serial.println(tCount);
  }
}


extern "C" void TIMER2_IRQHandler() {
  if (NRF_TIMER2->EVENTS_COMPARE[0]){
    NRF_TIMER2->EVENTS_COMPARE[0] = 0; // clear event flag
    tCount++;
  }
}


void setTimer2(bool enable) {
  // if timer enabled start it, setup lieke timer 1 in previous tasks
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
  // disable timer
  else{
    NRF_TIMER2->TASKS_STOP = 1;
    NVIC_DisableIRQ(TIMER2_IRQn);
  }
}
