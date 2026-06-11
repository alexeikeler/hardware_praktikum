// Task 2: Robust Temperature and Humidity Monitoring

#include <DHT.h>
#include <math.h>

// --- Configuration ---
// define sensor pin and type (DHT11)
#define DHTTYPE DHT11
#define DHTPIN D7

// --- Objects ---
// create DHT sensor instance
DHT dht(DHTPIN, DHTTYPE);
// --- Timing ---
unsigned long lastSample = 0;
//  sampling interval (2 seconds)
#define intervalms 2000
int last_blink_ms = 0;

// --- State Variables ---
// store last valid temperature and humidity
float lastValidTemp;
float lastValidHumid;
// maintain a failure counter
float failure_counter = 0;


float a = 17.62;
float b = 243.12;
// --- Computation ---

float computeDewPoint(float tempC, float relHum) {
    //  Magnus formula using natural logarithm
    float gamma = log(relHum / 100) + (b * tempC) / (a + tempC);
    return (a * gamma) / (b - gamma);
}

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

}

void loop() {
    unsigned long now = millis();

    // TODO: implement non-blocking sampling (2 s)
    if (millis() - last_blink_ms  >= intervalms){
    last_blink_ms = millis();
    // TODO: read temperature and humidity
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();

    if (isnan(hum)){ 
        failure_counter +=1;
    }
    else{
        lastValidHumid = hum; // not nan hum is valid
        failure_counter = 0;
    }

    if (isnan(temp)){ 
        failure_counter +=1;
        }
    else{
        lastValidTemp = temp; // not nan to temp is valid
        failure_counter = 0;
    }


    Serial.print("T = ");
    Serial.print(lastValidTemp);
    Serial.print(" C    ");

    Serial.print("RH = ");
    Serial.print(lastValidHumid);
    Serial.print("%     ");

    float dew = computeDewPoint(lastValidTemp, lastValidHumid);
    Serial.print("dewPoint = ");
    Serial.print(dew);
    Serial.println(" C");

    if(failure_counter > 5){
        Serial.print( "[WARN] DHT failures: ");
        Serial.print(failure_counter);
    }

    }
    
}