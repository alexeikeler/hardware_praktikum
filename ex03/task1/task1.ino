// Task 1: Environmental Light Categorization
// Complete the implementation according to the task description.
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#define BLINK_INTERVAL_MS  5000UL 
// TODO: define necessary constants (e.g., calibration range, thresholds, timing)
int calibrationlow = 50;
int calibrationhigh = 3500;

unsigned long last_blink_ms = 0;


// TODO: implement a function that categorizes the normalized value
const char* categorize(int normalized) {
    if(normalized < 30){
        return "LOW";
    }
    else if(normalized < 70){
        return "MEDIUM";
    }
    else{
        return "HIGH";
    }
}

void setup() {
    // TODO: initialize Serial communication
    Serial.begin(115200);
    // TODO: configure ADC resolution to 12-bit
    analogReadResolution(12);


}

void loop() {
    // TODO: implement non-blocking timing using millis()
    // The system should sample every 500 ms
    if (millis() - last_blink_ms  >= BLINK_INTERVAL_MS){
    last_blink_ms = millis();
    // TODO: read raw value from light sensor
    int lightVal = analogRead(A0);
    // TODO: clamp the raw value to a calibrated range
    int constrainVal = constrain(lightVal, calibrationlow, calibrationhigh);
    // TODO: normalize the value to a 0–100 scale
    int normVal = map(constrainVal, calibrationlow,calibrationhigh, 0, 100);
    // TODO: determine the category using the categorize() function
    const char* category = categorize(normVal);
    // TODO: print raw value, normalized value, and category to Serial
    Serial.println(lightVal);
    Serial.println(normVal);
    Serial.println(category);
    }

}