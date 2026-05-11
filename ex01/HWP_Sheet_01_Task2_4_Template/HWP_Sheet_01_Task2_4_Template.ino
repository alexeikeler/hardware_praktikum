#include <Adafruit_TinyUSB.h>
#include <Wire.h>
#include <U8g2lib.h>


// Task 2 ii.) Constants — fill in the correct values from the datasheet
//             You can also write the answers to the questions here.
// ------------------------------------------------------------

// The address 58 belongs to the SGP30.
// two-byte command for initialization: Init_air_quality 2003
// two-byte command for measurement: Measure_air_quality 2008 
// Number of bytes a measurement response contains: 6,  
// Representation of each byte: The sensor responds with 2 data bytes (MSB first) and 1 CRC byte for each of the two 
// preprocessed air quality signals in the order CO2eq (ppm) and TVOC (ppb).

#define SGP30_ADDR       0x58   // 7-bit I2C address of the SGP30 

// Command codes (2 bytes each, MSB first — see datasheet )
#define CMD_INIT_MSB     0x20   //    first byte
#define CMD_INIT_LSB     0x03   //    second byte
#define CMD_MEAS_MSB     0x20
#define CMD_MEAS_LSB     0x08

// Display: air quality range for mapping CO2 to a percentage, you can change these to test more ranges
#define CO2_MIN          400    // ppm — clean outdoor air
#define CO2_MAX          2000   // ppm — poor indoor air quality
#define NEW_CO2_MIN 0
#define NEW_CO2_MAX 100

// Task 2 iv.)
#define INTERVAL_MS  1000UL
#define MEASUREMENTS_INVERVAL 12UL
#define MEASUREMENT_BYTES 6
#define VERBOSE false

unsigned long last_call = 0;
//uint16_t raw_co2_value = 0;
//uint16_t raw_tvoc_value = 0;
//uint8_t co2_percentage = 0;


// ------------------------------------------------------------
//  Display constructor
// ------------------------------------------------------------
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

//  Task 2 iii.) — Helper functions
  // ------------------------------------------------------------
  //  Raw byte storage for the last SGP30 measurement.
  //
  // We use global variables here to keep the function signatures simple. 
  // The cleaner alternatives (pointers or a result struct) use C concepts not yet introduced. 
  // At this scale globals are fine; in a larger project you would avoid them.
  //
  //  The SGP30 sends each 16-bit value as two separate bytes:
  //    MSB — most significant byte  (upper 8 bits of the value)
  //    LSB — least significant byte (lower 8 bits of the value)
  //  A third byte per value is a CRC checksum — we discard it.
  // ------------------------------------------------------------
uint8_t raw_co2_msb,  raw_co2_lsb;   // assign values with sgp30_read()
uint8_t raw_tvoc_msb, raw_tvoc_lsb;  // assign values with sgp30_read()


void sgp30_cmd(uint8_t msb, uint8_t lsb) {
  // ------------------------------------------------------------
  //  sgp30_cmd : send a 2-byte command to the SGP30
  //
  //  The SGP30 expects all commands as two bytes (MSB first).
  //  Example:  sgp30_cmd(CMD_INIT_MSB, CMD_INIT_LSB);
  // ------------------------------------------------------------
  // TODO: open a transmission to SGP30_ADDR,
  //       write msb, write lsb, close the transmission.
  //       
  Wire.beginTransmission(SGP30_ADDR);
  Wire.write(msb);
  Wire.write(lsb);
  Wire.endTransmission();
}


bool sgp30_read(uint8_t n) 
{
  // ------------------------------------------------------------
  //  sgp30_read : read one measurement(n bytes) from the SGP30.
  //
  //  Call sgp30_cmd() first, wait max measurement duration, 
  //  then call this function.
  //  The 4 raw bytes are stored in the global variables above.
  //  The two CRC bytes are read from the bus and discarded.
  //
  //  Returns true if all 6 bytes were received, false on error.
  // ------------------------------------------------------------
  // TODO: use Wire.requestFrom(SGP30_ADDR, n) to request n bytes.
  //       If the return value is not n, return false immediately. 
  //       The expected return value is in the Datasheet.
  //       Read the n bytes in order, remember wire.read() can only
  //       read one byte at a time. Look out for CRC bytes,  
  //       we don't need to store those.  
  //
  
  Wire.requestFrom(SGP30_ADDR, n);
  if (Wire.available() != n){
    return false;
  }

  raw_co2_msb = Wire.read();
  raw_co2_lsb = Wire.read();
  
  Wire.read();

  raw_tvoc_msb = Wire.read();
  raw_tvoc_lsb = Wire.read();

  Wire.read();

  if(VERBOSE)
  {
    Serial.println("-----------------");
    Serial.print("CO2 MSB: ");
    Serial.println(raw_co2_msb);
    Serial.print("CO2 LSB: ");
    Serial.println(raw_co2_lsb);
    Serial.println("-----------------");
    Serial.print("TVOC MSB: ");
    Serial.println(raw_tvoc_msb);
    Serial.print("TVOC LSB: ");
    Serial.println(raw_tvoc_lsb);
    Serial.println("-----------------");
  }

  return true;

}


uint16_t to_uint16(uint8_t msb, uint8_t lsb) {
  // ------------------------------------------------------------
  //  to_uint16 : combine two bytes into one 16-bit value.
  //
  //  A sensor value like CO2 = 450 ppm cannot fit in a single
  //  byte (max 255). The sensor splits it across two bytes:
  //    MSB holds the upper half: 450 >> 8  = 1   (0x01)
  //    LSB holds the lower half: 450 & 0xFF = 194 (0xC2)
  //
  //  To reconstruct the original value:
  //    shift MSB left by 8 bits  →  0x01 becomes 0x0100 (= 256)
  //    OR with LSB               →  0x0100 | 0xC2 = 0x01C2 (= 450)
  //
  //  The cast to uint16_t before shifting is necessary because
  //  uint8_t would overflow when shifted — always cast first.
  //
  // ------------------------------------------------------------
  return ((uint16_t)msb << 8) | lsb;
}



//  Task 3 — Display helper  (optional, but keeps loop() clean)
// void display_values(uint16_t co2, uint16_t tvoc) {
//   // ------------------------------------------------------------
//   //  display_values : show co2 and tvoc on the OLED
//   //
//   // ------------------------------------------------------------
//   // TODO (Task 3): set cursor, print co2 and tvoc values.
//   // TODO (Task 4): map co2 to pct (0-100), draw a filled bar with
//   //                u8g2.drawBox(x, y, width, height).
//   //                Bar width  = map(pct, 0, 100, 0, 128)
//   //                Remember: constrain pct to [0, 100] before mapping.
// }


// TODO (Task 3): set cursor, print co2 and tvoc values.
// TODO (Task 4): map co2 to pct (0-100), draw a filled bar with
//                u8g2.drawBox(x, y, width, height).
//                Bar width  = map(pct, 0, 100, 0, 128)
//                Remember: constrain pct to [0, 100] before mapping.
void update_display(uint16_t co2_value, uint16_t tvoc_value, uint8_t co2_percentage)
{
  u8g2.clearBuffer();

  // Draw CO2 value in ppm and %
  // --------------------------------------------------------------
  u8g2.setFont(u8g2_font_ncenB08_tf);
  u8g2.setCursor(0, 15);
  u8g2.print("CO");
  u8g2.setFont(u8g2_font_u8glib_4_tf);
  u8g2.setCursor(16, 15);
  u8g2.print(2);  
  
  u8g2.setFont(u8g2_font_ncenB08_tf);
  u8g2.setCursor(26, 15);
  u8g2.print(co2_value);
  u8g2.setCursor(55, 15);
  u8g2.print("ppm");
  u8g2.setCursor(90, 15);
  u8g2.print(co2_percentage);
  u8g2.setCursor(108, 15);
  u8g2.print("%");
  // --------------------------------------------------------------
  
  // Draw horizontal lines and represent percentage as afilled bar
  // on the display.
  //
  // Bar is a rectange with height=13px and width ~ 1.28 px,
  // but since only integer values are allowed it needs to be mapped
  // from [0, 100] to [0, 128] to cover the whole screen.
  //
  // --------------------------------------------------------------
  u8g2.setCursor(0, 18);
  u8g2.drawLine(0, 18, 128, 18);
  
  u8g2.setCursor(0, 20);
  u8g2.drawBox(0, 20, map(co2_percentage, 0, 100, 0, 128), 13);

  u8g2.setCursor(0, 34);
  u8g2.drawLine(0, 34, 128, 34);
  // --------------------------------------------------------------

  // Print TVOC value 
  // --------------------------------------------------------------
  u8g2.drawStr(20,55,"TVOC");
  u8g2.setCursor(55, 55);
  u8g2.print(tvoc_value);
  u8g2.setCursor(75, 55);
  u8g2.print("ppb");
  // --------------------------------------------------------------

  u8g2.sendBuffer();
}

void update_serial_monitor(uint16_t co2_value, uint16_t tvoc_value, uint8_t co2_percentage)
{
  Serial.println("------------------");
  Serial.print("C02: ");
  Serial.print(co2_value);
  Serial.print(" ppm ");
  Serial.print("(");
  Serial.print(co2_percentage);
  Serial.println(" %)");
  Serial.print("Tvoc: ");
  Serial.print(tvoc_value);
  Serial.println(" ppb");
  Serial.println("------------------");
}

void update_measurements(uint16_t co2_value, uint16_t tvoc_value, uint8_t co2_percentage)
{
  update_serial_monitor(co2_value, tvoc_value, co2_percentage);
  update_display(co2_value, tvoc_value, co2_percentage);
}


void setup() 
{
  Serial.begin(115200);
  while (!Serial);           // Wait for USB Serial connection
  // --------------------------------------------------------------------------------------------------
  //     Task 2 i.): I2C scanner
  // TODO: Transmit to each available I2C address, print the adress when receiving an ACK.
  //       You can use decimal adresses when sending but convert them to hex when printing them out.
  //       Use Serial.print(address, HEX) to make it easier.
  // --------------------------------------------------------------------------------------------------
  
  Wire.begin();
  u8g2.begin();
  
  // // 2 i.)
  Serial.println("Searching for addresses...");
  for (uint8_t addr = 8; addr < 128; addr++)
  {
    Wire.beginTransmission(addr);
    if(Wire.endTransmission() == 0)
    {
      Serial.print("Found address: 0x");
      Serial.println(addr, HEX);
    }
  }
  Serial.println("Done searching for addresses...");
  // found adresses: 3C, 51, 58

  
  // --- Task 3 i.): Simple display use ---
  // TODO: initialize display, set a font, display "Hardware Praktikum 2026",
  //       and push it to the screen.
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_squeezed_b6_tr);
  u8g2.drawStr(0,15,"Hardware Praktikum 2026");
  u8g2.drawStr(0,30,"Initializing spg30... (15s)");
  u8g2.sendBuffer();


  //     Task 2 iv.): Initialise SGP30 
  // TODO: send the init command, wait for initialization
  //       and print out a message.
  // --------------------------------------------------------------------------------------------------
  Serial.println("Initializing spg30... (15s)");
  sgp30_cmd(CMD_INIT_MSB, CMD_INIT_LSB);
  delay(15000);
  Serial.println("Initialization done");
}


void loop() {
 
  
  // --- Task 2 iv.): Send measure command and read response ---
  // TODO: call sgp30_cmd() with the measure command bytes.
  // TODO: call sgp30_read().
  //       If it returns false, print an error message and return early.
  // TODO: Reconstruct 16-bit values from the raw bytes ----
  // TODO: print co2 and tvoc with appropriate labels and units.
  if (millis() - last_call >= INTERVAL_MS){
      last_call = millis();

      sgp30_cmd(CMD_MEAS_MSB, CMD_MEAS_LSB);
      
      delay(MEASUREMENTS_INVERVAL);
  
      if(!sgp30_read(MEASUREMENT_BYTES))
      {
        Serial.println("Measurement failed");
        return;
      }
      
      // --- Task 3 ii.): Print the sgp30 values on the display
      //                  in addition to the Serial monitor 
      // --- Task 4: Map CO2 to a percentage ---
      // TODO: use map() to scale co2 from raw values to 0-100%.
      //       Then use constrain() to make sure the percetange 
      //       doesnt go outside 0-100.
      //
      // Both tasks are done below.
      
      uint16_t raw_co2_value = to_uint16(raw_co2_msb, raw_co2_lsb);
      uint16_t raw_tvoc_value = to_uint16(raw_tvoc_msb, raw_tvoc_lsb);  
      uint8_t co2_percentage = constrain(
        map(raw_co2_value, CO2_MIN, CO2_MAX, NEW_CO2_MIN, NEW_CO2_MAX), 
        NEW_CO2_MIN, NEW_CO2_MAX
      );
      
      update_measurements(raw_co2_value, raw_tvoc_value, co2_percentage);

  }
}