// Task 1: Environmental Light Categorization
// Complete the implementation according to the task description.

// TODO: define necessary constants (e.g., calibration range, thresholds, timing)
const int LIGHT_SENSOR_PIN = 0;
const int CALIB_MIN = 50;
const int CALIB_MAX = 3500;
const int NORMALIZE_MIN = 0;
const int NORMALIZE_MAX = 100;

// TODO: implement a function that categorizes the normalized value
const char* categorize(int normalized) {
    if (normalized < 30){
        return "LOW";
    }
    else if (normalized < 70){
        return "MEDIUM";
    }
    else {
        return "HIGH";
    }
    // TODO: return "LOW", "MEDIUM", or "HIGH"
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

    // TODO: read raw value from light sensor
    int rawVal = analogRead(LIGHT_SENSOR_PIN);

    // TODO: clamp the raw value to a calibrated range
    int clampVal = constrain(rawVal, CALIB_MIN, CALIB_MAX);
    // TODO: normalize the value to a 0–100 scale
    int normalizedVal = map(clampVal, CALIB_MIN, CALIB_MAX, NORMALIZE_MIN, NORMALIZE_MAX);

    // TODO: determine the category using the categorize() function
    const char* category = categorize(normalizedVal);
    // TODO: print raw value, normalized value, and category to Serial
}