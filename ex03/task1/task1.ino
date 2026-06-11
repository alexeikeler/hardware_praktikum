// Task 1: Environmental Light Categorization
// Complete the implementation according to the task description.
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#define SAMPLE_INTERVAL_MS  5000UL 
// define necessary constants (e.g., calibration range, thresholds, timing)
int calibrationlow = 50;
int calibrationhigh = 3500;

// scale constants
int scaleLow = 0;
int scaleHigh = 100;

// counter for the interval
unsigned long last_count_ms = 0;


// function that categorizes the normalized value
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
    // initialize Serial communication
    Serial.begin(115200);
    //configure ADC resolution to 12-bit
    analogReadResolution(12);
}

void loop() {
    // The system samples every 500 ms
    if (millis() - last_count_ms  >= SAMPLE_INTERVAL_MS){
    last_count_ms = millis();

    // read raw value from light sensor
    int lightVal = analogRead(A0);
    // clamp the raw value to a calibrated range
    int constrainVal = constrain(lightVal, calibrationlow, calibrationhigh);
    // normalize the value to a 0–100 scale
    int normVal = map(constrainVal, calibrationlow,calibrationhigh, scaleLow, scaleHigh);
    // determine the category using the categorize() function
    const char* category = categorize(normVal);

    // print raw value, normalized value, and category to Serial
    Serial.print("raw Value:  ");
    Serial.println(lightVal);
    Serial.print("normalized Value:  ");
    Serial.println(normVal);
    Serial.print("category:  ");
    Serial.println(category);

    }

}