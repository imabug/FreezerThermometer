/*
Freezer thermometer
 Uses an Analog Devices TMP36 temperature sensor and displays
 the current temperature on an Adafruit RGB LCD shield
 */

// include the library code:
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

// For the TMP36 temperature sensor
#define aref_voltage 3.3         // we tie 3.3V to ARef and measure it with a multimeter!

void setup() {
  // analog pin for the TMP36 Vout pin
  // resolution is 10 mV/deg C with a 500 mV offset to allow for
  // negative temperatures
  int tempPin = 1;
  int tempReading;

  // Debugging output
  // Serial.begin(9600);
  // Get an LCD object
  Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // If you want to set the aref to something other than 5v
  analogReference(EXTERNAL);

  lcd.print("Current temp: ");
  lcd.setBacklight(WHITE);
}

void loop(void) {
  tempReading = analogRead(tempPin);
  // converting that reading to voltage, which is based off the reference voltage
  float voltage = (tempReading * aref_voltage)/1024.0;
  // now calculate the temperature from the measured voltage
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
  //to degrees ((volatge - 500mV) times 100)

  // Display the current temperature on the second line of LCD display
  lcd.setCursor(0, 1);
  lcd.print(temperatureC);
  lcd.print(" degC");

  // Wait 5 minutes before making the next measurement
  delay(5*60*1000);
}

