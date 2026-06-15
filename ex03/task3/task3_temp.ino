// Task 3: Air Quality Monitoring with SGP30


#include <Adafruit_SGP30.h>

// --- Objects ---
Adafruit_SGP30 sgp;

// --- Timing ---
// define sampling interval (1 second)
#define SAMPLE_INTERVAL_MS  1000UL 


unsigned long last_sample_ms = 0;  

// --- Warm-up ---
// define warm-up duration (15 seconds)
#define WARMUP_INTERVAL_MS 15000UL
// maintain a warm-up state indicator
bool ready = false;

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    // initialize the sensor and handle initialization failure
    bool initialized = sgp.begin();
    if (!initialized){
        Serial.println("spg initialization failed");
        return;
    }
    // start air quality measurements
    sgp.IAQinit();

    // TODO: record system start time
}

void loop() {
    unsigned long now = millis();

    // implement periodic sampling at 1 Hz using millis()
    // perform a measurement and handle failure cases
    // update warm-up state based on elapsed time
    if (now - last_sample_ms  >= SAMPLE_INTERVAL_MS){
        last_sample_ms = now;
        bool measured = sgp.IAQmeasure();
        ready = now >=  WARMUP_INTERVAL_MS; 

        if (!measured) {
            Serial.println("Measurement failed");
        }
        else {
            if (!ready){
                Serial.println("UNSTABLE/WARMUP");
            }
            else {
                Serial.println("Sensor ready");
             }
            // output measurement results
            //       - indicate warm-up vs ready state
            //       - ensure correct formatting for integer values
            Serial.print("eCO2:"); 
            Serial.print(sgp.eCO2);
            Serial.print("ppm\tTVOC:"); 
            Serial.print(sgp.TVOC);
            Serial.println("ppb");
        }
    }

}