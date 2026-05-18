#include <Adafruit_TinyUSB.h>

// ------------------------------------------------------------
//  Task 1:
//      Write the body of setP026() as specified in the exercise sheet.
//      To toggle at the right frequency, you can use
//      this function in a loop.
// ------------------------------------------------------------
const uint8_t PIN_026_BIT = 26;

void setP026(boolean high) {
  if (high) {
    NRF_P0->OUTSET = (1UL << PIN_026_BIT);
  }
  else {
    NRF_P0->OUTCLR = (1UL << PIN_026_BIT);
  }
}

void setup() {
  // Without this line after every upload the
  // microcontroller cannot be seen by the arduino IDE
  Serial.begin(115200);

  // Set P0.26 as output
  NRF_P0->DIRSET = (1UL << PIN_026_BIT);
}


void loop() {
  // Since we must blink at 1 Hz, ON/OFF time should be 0.5 sec.
  setP026(true);
  delay(500UL);
  setP026(false);
  delay(500UL);
}
