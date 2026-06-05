
// ------------------------------------------------------------
//  Task 4:
//      Make necessary changes to your code from Task 3. Provided tests
//      do not cover all cases.
// ------------------------------------------------------------

#include <Arduino.h>
volatile bool buzzerHigh = false;

void setup() {
  NRF_P0->DIRSET = (1UL << 29); // Set P0.29 as output
  // configure the timer 
  setupTimer1(); 

}

// Timer 1 doesnt need to be reset every time
161718192021222324252627282930313233343536373839404142434445464748

// ------------------------------------------------------------
//  Task 4:
//      Make necessary changes to your code from Task 3. Provided tests
//      do not cover all cases.
// ------------------------------------------------------------

#include <Arduino.h>
volatile bool buzzerHigh = false;


Sketch uses 21300 bytes (2%) of program storage space. Maximum is 811008 bytes.
Global variables use 3100 bytes (1%) of dynamic memory, leaving 234468 bytes for local variables. Maximum is 237568 bytes.
Upgrading target on COM7 with DFU package C:\Users\preus\AppData\Local\arduino\sketches\EB5942EE11F612759BB1316C14DD49AE\task4.ino.zip. Flow control is disabled, Single bank, Touch disabled
########################################
##
Activating new firmware
Device programmed.


void setupTimer1() {
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
  // if frequency out of range stop playing anything
  if (freq < 100 || freq > 3000){
    NRF_TIMER1->TASKS_STOP = 1;
    NRF_P0->OUTCLR = (1UL << 29);
    buzzerHigh = false;
    return;
  }
  // timer clock is 1 MHz(16MHZ / 2^prescaler); we need half period -> 2*freq
  NRF_TIMER1->CC[0] = 1000000/(2 * freq);

  // restart timer with new compare val
  NRF_TIMER1->TASKS_CLEAR = 1;
  NRF_TIMER1->TASKS_START = 1;

}


extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]){
    NRF_TIMER1->EVENTS_COMPARE[0] = 0; // reset flag 
    // change output between high and low for wave form
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