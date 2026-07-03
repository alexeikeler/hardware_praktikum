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
#include <ArduinoBLE.h>

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
// Create variables to store last detected gestures
// - lastDetectedGesture: from accelerometer FSM
String lastDetectedGesture = "NONE";
// - lastDynamicGesture: from gyroscope
String lastDynamicGesture = "NONE";



// =====================
// Part E: BLE Objects
// =====================
// Create BLE service and characteristic
BLEService imuService("180C");
BLECharacteristic imuCharacteristic("2A56", BLERead | BLENotify, 150);


// =====================
// Part B: Orientation Detection
// =====================
String detectOrientation(float ax, float ay, float az) {

  // Detect device orientation from accelerometer
  // Use thresholds on ay and az axes
   // az is used for FACE UP / FACE DOWN, ay is used for LEFT SIDE/ RIGHTSIDE
  float threshold = 8.5;
   // Compare absolute values to select the dominant orientation axis.
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

// find min and max az values in the window
// a large range indicates that the device was rotated
  for(int x = 1; x < WINDOW_SIZE; x++){
    if (az_buffer[x] < minaz){
      minaz = az_buffer[x];
    }
    if (az_buffer[x] > maxaz){
      maxaz = az_buffer[x];
    }
  }
 // Use the difference between min and max for motion detection
 // the first and last buffer values are compared to estimate the rotation direction
  float thres = 16.0;
  if (maxaz - minaz >= thres && az_buffer[0] > az_buffer[WINDOW_SIZE -1]){
    return "Pronation";
  }
  if(maxaz - minaz >= thres && az_buffer[0] <= az_buffer[WINDOW_SIZE- 1]){
    return "Supination";
  }

  return "NONE";
}


// =====================
// Part D: Gyroscope-based Dynamic Gesture Detection
// =====================
String detectDynamicGesture(float gyrX, float gyrY, float gyrZ) {
  // Detect 6 gestures using gyroscope
  // Gestures: TILT_LEFT, TILT_RIGHT, MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT
  // Return strongest gesture (highest magnitude)
  
  float thres = 100;
  // The X-axis showed noticeably larger gyroscope values on our sensor setup
  // therefore, a higher threshold is used for tilt detection to avoid
  // classifying too many movements as TILT_LEFT or TILT_RIGHT
  if( abs(gyrX) < thres*1.5  && abs(gyrY) < thres && abs(gyrZ)< thres){
    return "NONE";
  }
   // Compare absolute gyroscope values to detect the strongest movement axis
   // the sign of the strongest axis determines the movement direction
  if(abs(gyrX) > abs(gyrY) && abs(gyrX) > abs(gyrZ)){
    if (gyrX >=  0){
      return "TILT_RIGHT";
    }
    return "TILT_LEFT";
  }
  if(abs(gyrY) > abs(gyrX) && abs(gyrY) > abs(gyrZ)){
    if (gyrY >=  0){
      return "MOVE_DOWN";
    }
    return "MOVE_UP";
  }
  if(abs(gyrZ) > abs(gyrY) && abs(gyrZ) > abs(gyrX)){
    if (gyrZ >=  0){
      return "MOVE_LEFT";
    }
    return "MOVE_RIGHT";
  }
  

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
  // Initialize BLE, add service/characteristic, start advertising
  if (!BLE.begin()) {
  Serial.println("starting Bluetooth Low Energy module failed!");
  while (1);
  }

  BLE.setConnectionInterval(6, 12);
  BLE.setLocalName("XIAO-IMU");

  imuService.addCharacteristic(imuCharacteristic);
  BLE.addService(imuService);

  // set the initial value for the characteristic
  imuCharacteristic.writeValue("IMU ready");

  // start advertising
  BLE.advertise();

  Serial.println("BLE advertising started.");
}


void loop() {
  BLE.poll(); // processes BLE events so the phone can connect and receive notifications.
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
    // Call detectDynamicGesture()
    // If gesture detected (not "NONE"), store in lastDynamicGesture
    String dynamicGesture = detectDynamicGesture(gyrX, gyrY,gyrZ); 
    if(dynamicGesture != "NONE"){
      lastDynamicGesture = dynamicGesture;
    }

    // =====================
    // Serial Output (USB)
    // =====================
  Serial.print("ax:"); Serial.print(ax, 2);
  Serial.print(" | ay:"); Serial.print(ay, 2);
  Serial.print(" | az:"); Serial.print(az, 2);

  Serial.print(" | gyrX:"); Serial.print(gyrX, 2);
  Serial.print(" | gyrY:"); Serial.print(gyrY, 2);
  Serial.print(" | gyrZ:"); Serial.println(gyrZ, 2);

  Serial.print("Orientation: ");
  Serial.print(orientation);

  Serial.print(" | Acc Gesture: ");
  Serial.print(lastDetectedGesture);

  Serial.print(" | Gyro Gesture: ");
  Serial.println(lastDynamicGesture);
  
    // =====================
    // Part E: Bluetooth Low Energy (BLE) Communication
    // =====================
    // Format and send data via BLE:
    // - Accelerometer readings (ax, ay, az)
    // - Gyroscope readings (gyrX, gyrY, gyrZ)
    // - Orientation detection result
    // - Gesture detection results (FSM and Gyro)
    String bleData =
   "ax:" + String(ax, 2) +
    " | ay:" + String(ay, 2) +
    " | az:" + String(az, 2) +
    " | gyrX:" + String(gyrX, 2) +
    " | gyrY:" + String(gyrY, 2) +
    " | gyrZ:" + String(gyrZ, 2) +
    "\nOrientation: " + orientation +
    " | Acc Gesture: " + lastDetectedGesture +
    " | Gyro Gesture: " + lastDynamicGesture;

  // c_str() converts the Arduino String into a C-style string required by writeValue()
  imuCharacteristic.writeValue(bleData.c_str());
  }
}