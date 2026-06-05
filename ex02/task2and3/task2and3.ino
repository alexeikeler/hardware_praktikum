
// ------------------------------------------------------------
//  Task 2 and 3:
//      Write the body of setTimer1Freq() as specified in the exercise sheet.
//      This should include timer settings.
//      Implement the ISR in TIMER1_IRQHandler().
//      Make necessary changes for setBuzzerFreq()
// ------------------------------------------------------------

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

volatile bool buzzerHigh = false;
volatile bool buzzerEnabled = false;

void setup() {
  Serial.begin(115200);
  pinMode(1, INPUT_PULLUP);
  NRF_P0->DIRSET = (1UL << 29); // Set P0.29 as output
}


void loop() {
  setBuzzerFreq();
}



/* void setTimer1Freq() {
  // configure timer
  NRF_TIMER1->TASKS_STOP = 1;
  NRF_TIMER1->TASKS_CLEAR = 1;
  NRF_TIMER1->MODE = 0;
  NRF_TIMER1->BITMODE = 3;
  NRF_TIMER1->PRESCALER = 4;
  // 478 = 16MHZ / 2^prescaler/ 2* freqency
  NRF_TIMER1->CC[0] = 478; 
  NRF_TIMER1->SHORTS = (1UL << 0);
  NRF_TIMER1->INTENSET = (1UL << 16);
  NVIC_EnableIRQ(TIMER1_IRQn); // enable interrupts
  NRF_TIMER1->TASKS_START = 1; // start timer
} */


// only plays tone when the button is being pressed
void setBuzzerFreq() {
  bool buttonPressed = !(NRF_P0->IN & (1UL << 3)); 
  // play tone if button pressed ; starts timer
  if (buttonPressed && !buzzerEnabled){
    // configure timer
    NRF_TIMER1->TASKS_STOP = 1;
    NRF_TIMER1->TASKS_CLEAR = 1;
    NRF_TIMER1->MODE = 0;
    NRF_TIMER1->BITMODE = 3;
    NRF_TIMER1->PRESCALER = 4;
    NRF_TIMER1->CC[0] = 478;
    NRF_TIMER1->SHORTS = (1UL << 0);
    NRF_TIMER1->INTENSET = (1UL << 16);
    NVIC_EnableIRQ(TIMER1_IRQn); // enable interrupts
    NRF_TIMER1->TASKS_START = 1;
    buzzerEnabled = true;
  }
  // no tone playing when button not pressed ; stops timer
  if (!buttonPressed && buzzerEnabled) {
    NRF_TIMER1->TASKS_STOP = 1;
    NRF_TIMER1->INTENCLR = (1UL << 16);
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    NRF_P0->OUTCLR = (1UL << 29);
    buzzerHigh = false;
    buzzerEnabled = false;
  }
}


extern "C" void TIMER1_IRQHandler() {
  if (NRF_TIMER1->EVENTS_COMPARE[0]){
    NRF_TIMER1->EVENTS_COMPARE[0] = 0; // reset event flag
    // turn buzzer off if on and vice versa
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



