/*
 * Exercise 04: Wearable Context Recognition with IMU
 * Author: Roozbeh Ghazavi
 * Year: 2026
 * Board: XIAO nRF52840 Sense
 * Sensor: LSM6DS3
 *
 * Implement ALL TODO sections.
 */

#include <Arduino.h>
#include "LSM6DS3.h"
#include "Wire.h"

// =====================
// Part E: BLE Library
// =====================
// #include <ArduinoBLE.h>

LSM6DS3 myIMU(I2C_MODE, 0x6A);

// Constants
#define CONVERT_G_TO_MS2 9.80665f
#define FREQUENCY_HZ 50
#define INTERVAL_MS (1000 / (FREQUENCY_HZ + 1))

static unsigned long last_interval_ms = 0;

// =====================
// Part C: Accelerometer Buffer (Z-axis)
// =====================
#define WINDOW_SIZE 10

float az_buffer[WINDOW_SIZE];
int buffer_index = 0;
bool buffer_full = false;


// =====================
// Part C & D: Last Detected Gestures
// =====================
// TODO: Create variables to store last detected gestures
// - lastDetectedGesture: from accelerometer FSM
String lastDetectedGesture = "NONE";
// - lastDynamicGesture: from gyroscope
String lastDynamicGesture = "NONE";



// =====================
// Part E: BLE Objects
// =====================
// TODO: Create BLE service and characteristic
// Example:
// BLEService imuService("180C");
// BLECharacteristic imuCharacteristic("2A56", BLERead | BLENotify, 100);


// =====================
// Part B: Orientation Detection
// =====================
String detectOrientation(float ax, float ay, float az) {

  // Detect device orientation from accelerometer
  // Use thresholds on ay and az axes
  float threshold = 8.5;
  if ((abs(az) >= abs(ay)) && abs(az) >= threshold){
    if (az > 0) {
      return "FACE UP";
    }
    else {
      return "FACE DOWN";
    }
  }

  else if (abs(ay) >= threshold){
    if (ay > 0) {
      return "RIGHT SIDE";
    }
    else {
      return "LEFT SIDE";
    }
  }


  return "UNKNOWN";
}


// =====================
// Part C: Accelerometer-based Gesture Detection
// =====================

// since the buffer window is very short you need to turn the sensor really fast to get a good measurement
String detectGestureWindow() {

  // Detect SUPINATION/PRONATION from Z-axis acceleration buffer
  // Analyze min/max range and motion direction

  if(!buffer_full){
      return "NONE";
  }

  float maxaz = az_buffer[0];
  float minaz = az_buffer[0];

// find min and max
  for(int x = 1; x < WINDOW_SIZE; x++){
    if (az_buffer[x] < minaz){
      minaz = az_buffer[x];
    }
    if (az_buffer[x] > maxaz){
      maxaz = az_buffer[x];
    }
  }
  // compare last and first elt of buffer to determine direction
  // difference of min and max for motion detection
  float thres = 16.0;
  if (maxaz - minaz >= thres && az_buffer[0] > az_buffer[WINDOW_SIZE -1]){
    return "Pronation";
  }
  if(maxaz - minaz >= thres && az_buffer[0] <= az_buffer[WINDOW_SIZE- 1]){
    return "Subpination";
  }

  return "NONE";
}


// =====================
// Part D: Gyroscope-based Dynamic Gesture Detection
// =====================
String detectDynamicGesture(float gyrX, float gyrY, float gyrZ) {
  float thres = 100;
  if(abs(gyrX) > abs(gyrY) && abs(gyrX) > abs(gyrZ)){
    if (gyrX >=  0){
      return "TILT_RIGHT";
    }
    return "TILT_LEFT";
  }
  if(abs(gyrY) > abs(gyrX) && abs(gyrY) > abs(gyrZ)){
    if (gyrX >=  0){
      return "MOVE_DOWN";
    }
    return "MOVE_UP";
  }
  if(abs(gyrZ) > abs(gyrY) && abs(gyrZ) > abs(gyrX)){
    if (gyrX >=  0){
      return "MOVE_RIGT";
    }
    return "MOVE_LEFT";
  }
  // TODO: Detect 6 gestures using gyroscope
  // Gestures: TILT_LEFT, TILT_RIGHT, MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT
  // Return strongest gesture (highest magnitude)

  return "NONE";
}


// =====================
// Part C (Bonus): Gesture State Machine (FSM)
// =====================
enum State {
  IDLE,
  MOVING,
  DETECTED
};

State currentState = IDLE;

String detectGestureFSM() {

  // TODO: Implement FSM for robust gesture detection
  // Transitions: IDLE → MOVING → DETECTED → IDLE
  // Store detected gesture in lastDetectedGesture

  return "NONE";
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (myIMU.begin() != 0) {
    Serial.println("IMU initialization failed!");
    while (1);
  }

  Serial.println("IMU initialized.");

  // =====================
  // Part E: BLE Setup
  // =====================
  // TODO: Initialize BLE, add service/characteristic, start advertising
}


void loop() {
  if (millis() > last_interval_ms + INTERVAL_MS) {
    last_interval_ms = millis();

    // =====================
    // Part A: IMU Data Acquisition
    // =====================
    // Read accelerometer (ax, ay, az) from myIMU
    // Convert accelerometer from G to m/s² using CONVERT_G_TO_MS2
   
    float ax = myIMU.readFloatAccelX() * CONVERT_G_TO_MS2;
    float ay = myIMU.readFloatAccelY() * CONVERT_G_TO_MS2;
    float az = myIMU.readFloatAccelZ() * CONVERT_G_TO_MS2;

    // Read gyroscope (gyrX, gyrY, gyrZ) from myIMU

    float gyrX = myIMU.readFloatGyroX();
    float gyrY = myIMU.readFloatGyroY();
    float gyrZ = myIMU.readFloatGyroZ();


    // =====================
    // Part C: Accelerometer Buffer Management
    // =====================
    az_buffer[buffer_index] = az;
    buffer_index++;

    if (buffer_index >= WINDOW_SIZE) {
      buffer_index = 0;
      buffer_full = true;
    }


    // =====================
    // Part B: Orientation Detection
    // =====================
    String orientation = detectOrientation(ax, ay, az);


    // =====================
    // Part C: Accelerometer-based Gesture Detection
    // =====================
    // Call detectGestureWindow()
    String detectedGesture = detectGestureWindow() ;  
    // If gesture detected (not "NONE"), store in lastDetectedGesture
    if(detectedGesture != "NONE"){
      lastDetectedGesture = detectedGesture;
    }
    // =====================
    // Part D: Gyroscope-based Dynamic Gesture Detection
    // =====================
    // TODO: Call detectDynamicGesture()
    // TODO: If gesture detected (not "NONE"), store in lastDynamicGesture
    String dynamicGesture = detectDynamicGesture(gyrX, gyrY,gyrZ); 
    if(dynamicGesture != "NONE"){
      lastDynamicGesture = dynamicGesture;
    }

    // =====================
    // Serial Output (USB)
    // =====================
    Serial.println("ax: "); Serial.print(ax);
    Serial.println(" | ay: "); Serial.print(ay);
    Serial.println(" | az: "); Serial.print(az);

    Serial.println(" | gyrX: "); Serial.print(gyrX);
    Serial.println(" | gyrY: "); Serial.print(gyrY);
    Serial.println(" | gyrZ: "); Serial.print(gyrZ);

    Serial.println(" | az: "); Serial.print(az);
    Serial.println(" | Orientation: ");
    Serial.println(orientation);

    Serial.println(" | Accelerometer Gesture: ");
    Serial.println(lastDetectedGesture);

    Serial.println(" | Gyro Gesture: ");
    Serial.println(lastDynamicGesture);
  

    // =====================
    // Part E: Bluetooth Low Energy (BLE) Communication
    // =====================
    // TODO: Format and send data via BLE:
    // - Accelerometer readings (ax, ay, az)
    // - Gyroscope readings (gyrX, gyrY, gyrZ)
    // - Orientation detection result
    // - Gesture detection results (FSM and Gyro)
  }
}