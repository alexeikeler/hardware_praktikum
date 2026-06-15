// Task 4: Smart Plant Monitoring Station

#include <ArduinoBLE.h>
#include <Wire.h>
#include <Adafruit_SGP30.h>
#include <U8g2lib.h>
#include <DHT.h>
#include <string.h>

// --- Hardware Configuration ---
// TODO: define pins (DHT, light sensor, LED, buzzer)
#define DHTTYPE DHT11
#define DHTPIN D7
#define ANALOG_RESOLUTION_BITS 12
#define LIGHT_SENSOR_PIN A0
#define NUMBER_OF_LINES 5
#define MESSAGE_LENGTH 128
#define BUZZER_PIN D3

//----------------------------------------------------------------
// --- System Constants ---
// Scoring boundaries
const uint8_t LIGHT_LEFT_BOUNDARY = 25;
const uint8_t LIGHT_RIGHT_BOUNDARY = 90;
const uint8_t CALIBRATION_LOW = 50;
const uint16_t CALIBRATION_HIGH = 3500;
const uint8_t SCALE_LOW = 0;
const uint8_t SCALE_HIGH = 100;

const uint8_t TEMP_LEFT_BOUNDARY = 18;
const uint8_t TEMP_RIGHT_BOUNDARY = 30;

const uint8_t HUM_LEFT_BOUNDARY = 30;
const uint8_t HUM_RIGHT_BOUNDARY = 75;

const uint16_t AIR_Q_SCR_BOUNDARY = 1200;
const uint16_t AIR_Q_BAD = 2200;

const uint16_t ATTENTION_LED_INTERVAL = 500UL;
const uint16_t STRESSED_LED_INTERVAL = 250UL;

//----------------------------------------------------------------
// TODO: define timing constants for:

// - sensor sampling
const uint16_t WARMUP = 30000UL;
const uint16_t LIGHT_SENSOR_SI = 500UL;
const uint16_t AIR_SI = 1000UL;
const uint16_t TEMP_AND_HUM_SI = 2000UL;
// Buzzer
const uint16_t BUZZER_FREQ = 440;
const uint16_t BUZZER_DURATION = 1000UL;


// - display refresh (~2 Hz)
// - BLE transmission (1 Hz)
// - warm-up duration (30 s)
//----------------------------------------------------------------
// --- Objects ---
// TODO: initialize display, sensors, BLE service and characteristic
// --- FSM ---
enum SystemState {
  STATE_INIT,
  STATE_HEALTHY,
  STATE_ATTENTION,
  STATE_STRESSED
};

struct Humidity {
  float current;
  float lastValid;
  bool isCurrentNan;
};

struct Temperature {
  float current;
  float lastValid;
  bool isCurrentNan;
};

BLEService service("19B10000-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite | BLENotify, MESSAGE_LENGTH);
BLEDevice central;

Adafruit_SGP30 sgp30;
DHT dht11(DHTPIN, DHTTYPE);
SystemState currentState = STATE_INIT;
Humidity hum = { 0, 0 };
Temperature temp = { 0, 0 };
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

const uint8_t COL_DISTANCE = 8;
char messageStrings[NUMBER_OF_LINES][MESSAGE_LENGTH];
char bleMessage[MESSAGE_LENGTH];

unsigned long sgp30_lst = 0;
unsigned long lightSensor_lst = 0;
unsigned long dht11_lst = 0;
uint8_t lightValue;
unsigned long led_lst = 0;
//----------------------------------------------------------------

//----------------------------------------------------------------

// TODO: define timing variables for asynchronous operation

// TODO: define variables for sensor data storage

// TODO: helper function(s), e.g.:
// - state to string conversion
// - value normalization (light)

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000)
    ;
  // TODO: initialize hardware (pins, display, sensors)
  u8g2.begin();

  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, HIGH);

  // TODO: initialize BLE and start advertising
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
  }
  BLE.setConnectionInterval(6, 12);
  BLE.setLocalName("SmartPlantMonitoringSystem");
  service.addCharacteristic(switchCharacteristic);
  BLE.addService(service);
  // set the initial value for the characeristic:
  //switchCharacteristic.writeValue(0);
  // start advertising
  BLE.advertise();


  // TODO: store system start time (for warm-up)

  //----------------------------------------------------------------
  analogReadResolution(ANALOG_RESOLUTION_BITS);
  //----------------------------------------------------------------


  // SGP 30
  //----------------------------------------------------------------
  if (!sgp30.begin()) {
    Serial.println("SGP30 initialization failed!");
    return;
  } else {
    sgp30.IAQinit();
  }

  if (currentState == STATE_INIT) {
    while (millis() < WARMUP) {
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_squeezed_b6_tr);
      u8g2.drawStr(0, 30, "Warming up ... (30s)");
      u8g2.sendBuffer();
    }
  }
}

// Map state to the appropriate character.
// Initialy it wasn't this short, but BLE transmission doesn't work 
// correctly if the message is too long.
const char* getState() 
{
  switch (currentState) {
    case STATE_INIT: return "I";
    case STATE_HEALTHY: return "H";
    case STATE_ATTENTION: return "A";
    case STATE_STRESSED: return "S";
  }
}

// Compute score based on collected data and change state.
void compute_and_transition(void) {
  uint8_t score = 0;

  if (lightValue >= LIGHT_LEFT_BOUNDARY && lightValue <= LIGHT_RIGHT_BOUNDARY) {
    score += 25;
  }
  if (temp.current >= TEMP_LEFT_BOUNDARY && temp.current <= TEMP_RIGHT_BOUNDARY) {
    score += 25;
  }
  if (hum.current >= HUM_LEFT_BOUNDARY && hum.current <= HUM_RIGHT_BOUNDARY) {
    score += 25;
  }
  if (sgp30.eCO2 < AIR_Q_SCR_BOUNDARY) {
    score += 25;
  }

  transition(score);
}

void transition(uint8_t& score) {
  if (sgp30.eCO2 > AIR_Q_BAD) {
    currentState = STATE_STRESSED;
  } else if (score >= 75) {
    currentState = STATE_HEALTHY;
  } else if (score >= 50 && score <= 74) {
    currentState = STATE_ATTENTION;
  } else {
    currentState = STATE_STRESSED;
  }
}


void update_display(void) {

  u8g2.clearBuffer();

  snprintf_P(
    messageStrings[0],
    MESSAGE_LENGTH,
    PSTR("State: %s"),
    getState());

  u8g2.drawStr(0, COL_DISTANCE, messageStrings[0]);

  snprintf_P(
    messageStrings[1],
    MESSAGE_LENGTH,
    PSTR("Temperature: %.2f C"),
    temp.isCurrentNan ? temp.lastValid : temp.current);

  u8g2.drawStr(0, COL_DISTANCE * 2, messageStrings[1]);


  snprintf_P(
    messageStrings[2],
    MESSAGE_LENGTH,
    PSTR("Humidity: %.2f %%"),
    hum.isCurrentNan ? hum.lastValid : hum.current);

  u8g2.drawStr(0, COL_DISTANCE * 3, messageStrings[2]);


  snprintf_P(
    messageStrings[3],
    MESSAGE_LENGTH,
    PSTR("Light value: %u"),
    lightValue);

  u8g2.drawStr(0, COL_DISTANCE * 4, messageStrings[3]);

  snprintf_P(
    messageStrings[4],
    MESSAGE_LENGTH,
    PSTR("Air quality: %u ppm"),
    sgp30.eCO2);
  u8g2.drawStr(0, COL_DISTANCE * 5, messageStrings[4]);

  u8g2.sendBuffer();
  return;
}

// Collect information from sensors
void get_light_measurements(void) {
  lightValue = map(
    constrain(analogRead(LIGHT_SENSOR_PIN), CALIBRATION_LOW, CALIBRATION_HIGH),
    CALIBRATION_LOW,
    CALIBRATION_HIGH,
    SCALE_LOW,
    SCALE_HIGH);
  return;
}

void get_sgp30_measurements(void) {
  if (!sgp30.IAQmeasure()) {
    Serial.println("SGP30 Measurement failed!");
  }
  return;
}

void get_dht11_measurements(void) {
  hum.current = dht11.readHumidity();
  temp.current = dht11.readTemperature();


  if (isnan(temp.current)) {
    Serial.print("[DHT11 WARNING] Temperature is NAN. Using last valid temeprature.");
    temp.isCurrentNan = true;
  } else {

    temp.lastValid = temp.current;
    temp.isCurrentNan = false;
  }
  if (isnan(hum.current)) {
    Serial.print("[DHT11 WARNING] Humidity is NAN. Using last valid humidity.");
    hum.isCurrentNan = true;

  } else {
    hum.lastValid = hum.current;
    hum.isCurrentNan = false;
  }
}


void toggle_led(const uint16_t &interval, unsigned long &currentTime)
{
  if(digitalRead(LED_GREEN) == HIGH && currentTime - led_lst >= interval)
  {
    led_lst = currentTime;
    digitalWrite(LED_GREEN, LOW);
  }
  else
  {
    digitalWrite(LED_GREEN, HIGH);
  }
}


void handle_current_state(unsigned long &currentTime)
{
  if(sgp30.eCO2 > AIR_Q_BAD)
  {
    tone(BUZZER_PIN, BUZZER_FREQ, BUZZER_DURATION);
  }

  // STATE_HEALTHY,
  // STATE_ATTENTION,
  // STATE_STRESSED

  switch(currentState)
  {
    case STATE_HEALTHY:
      if(digitalRead(LED_GREEN) == HIGH)
      {
        digitalWrite(LED_GREEN, LOW);
        Serial.println("In state healthy");
      }
      break;    
    case STATE_ATTENTION:
      toggle_led(ATTENTION_LED_INTERVAL, currentTime);
      Serial.println("In state attention");
      break;

    case STATE_STRESSED:
      toggle_led(STRESSED_LED_INTERVAL, currentTime);
      Serial.println("In state stressed");
      break;
  }

}

// Transmit gathered information
void ble_telemetry(void)
{
  if(!central || !central.connected())
  {
    central = BLE.central();
  }

  // if a central is connected to peripheral:
  if (central && central.connected()) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    
    snprintf_P(
        bleMessage,
        MESSAGE_LENGTH,
        PSTR("|%s|%.1f|%.1f|%i|%u"), // If the string is too long (>20bytes) it doesn't work
        getState(),
        temp.isCurrentNan ? temp.lastValid : temp.current,
        hum.isCurrentNan ? hum.lastValid : hum.current,
        lightValue,
        sgp30.eCO2
    );

    Serial.println(bleMessage);
    Serial.print("Length ");
    Serial.println(strlen(bleMessage));

      // while the central is still connected to peripheral
      // transmit the message
    while(central.connected()) {
          switchCharacteristic.writeValue((uint8_t *)bleMessage, strlen(bleMessage));          
          break;
        }
  }
}


void loop() {
  unsigned long now = millis();
  
  //----------------------------------------------------
  // 500 ms (0.5s)
  if (now - lightSensor_lst >= LIGHT_SENSOR_SI) {
    lightSensor_lst = now;
    get_light_measurements();
    update_display();
  }

  // 1000 ms (1s)
  // Get sgp30 values, compute score (since it lays in the same time interval)
  // and transition to the new state.
  if (now - sgp30_lst >= AIR_SI) {
    sgp30_lst = now;
    get_sgp30_measurements();

    // ------------------------------
    compute_and_transition();
    handle_current_state(now);
    ble_telemetry();
    // ------------------------------
  }

  // 2000 ms (2s)
  if (now - dht11_lst >= TEMP_AND_HUM_SI) {
    dht11_lst = now;
    get_dht11_measurements();
  }



  // TODO: maintain BLE stack (if required)

  // i) TODO: asynchronous sensor acquisition (light, DHT, SGP30)

  // iii) TODO: warm-up handling (STATE_INIT for 30 s)

  // iii) TODO: compute health score (0–100)

  // iii) TODO: implement state transitions (including critical override)

  // iv) TODO: update OLED display (~2 Hz)

  // v) TODO: implement LED and buzzer behavior (non-blocking)

  // vi) TODO: send BLE telemetry (formatted string, 1 Hz)
}