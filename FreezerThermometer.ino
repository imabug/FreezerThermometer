/*
Freezer thermometer
 Uses an Analog Devices TMP36 temperature sensor and displays
 the current temperature on an Adafruit RGB LCD shield
*/

// include the library code:
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

#define DEBUG 1

// These #defines make it easy to set the backlight color
#define BLACK 0x0
#define RED 0x1
#define GREEN 0x2
#define YELLOW 0x3
#define BLUE 0x4
#define VIOLET 0x5
#define TEAL 0x6
#define WHITE 0x7

// For the TMP36 temperature sensor
#define aref_voltage 3.3         // we tie 3.3V to ARef and measure it with a multimeter!

// analog pin for the TMP36 Vout pin
// resolution is 10 mV/deg C with a 500 mV offset to allow for
// negative temperatures
int tempPin = 1;
int tempReading;
unsigned long sleepTime = 60*1000;    // delay interval before reading the TMP36 in ms
unsigned long dispTime = 30*1000;     // sleep interval
float voltage;
float temperatureC;
float minTemp;
float maxTemp;
int i;

// Get an LCD object
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup() {
  
  // Debugging output
  Serial.begin(9600);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // If you want to set the aref to something other than 5v
  analogReference(EXTERNAL);

  lcd.print("Current temp: ");
  lcd.setBacklight(WHITE);
  
  minTemp = (((analogRead(tempPin) * aref_voltage)/1024.0) - 0.5)*100;
  maxTemp = minTemp;
}

void loop(void) {
  // Read the pin 5 times with a 100 ms delay between each reading and average them together
  tempReading = 0;
  for (i=0; i<5; i++) {
    tempReading += analogRead(tempPin);
    delay(100);
  }
  tempReading /= 5;

  // convert that reading to voltage, which is based off the reference voltage
  voltage = (tempReading * aref_voltage)/1024.0;

  // now calculate the temperature from the measured voltage
  temperatureC = (voltage - 0.5) * 100;  //converting from 10 mv per degree with 500 mV offset
  //to degrees ((volatge - 500mV) times 100)

  if (tempReading < minTemp) minTemp = temperatureC;
  if (tempReading > maxTemp) maxTemp = temperatureC;
  
#ifdef DEBUG
  Serial.print("Temp reading = ");
  Serial.print(tempReading);
  // print out the voltage
  Serial.print(" - ");
  Serial.print(voltage,4); Serial.println(" volts");
  Serial.print("Max value = ");
  Serial.println(maxTemp);
  Serial.print("Min value = ");
  Serial.println(minTemp);
#endif

  // Turn the screen on
  lcd.setBacklight(WHITE);    // Set display colour to white
  lcd.display();
  // Display the current temperature on the second line of LCD display
  lcd.setCursor(0, 1);
  lcd.print(temperatureC);
  lcd.print(" degC");

  delay(dispTime);            // Leave the display on for 30 seconds
  lcd.noDisplay();            // Turn the display off
  lcd.setBacklight(BLACK);    // turn the backlight off 
  delay(sleepTime);           // Wait sleepTime seconds for the next measurement
}

