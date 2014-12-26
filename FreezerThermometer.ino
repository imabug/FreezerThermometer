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
unsigned long dispTime = 30*1000;     // time to leave the LCD on for
unsigned long prevMillis = 0;
float currTemp;
float minTemp;
float maxTemp;
int buttons;

// Get an LCD object
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup() {
  // Debugging output
  Serial.begin(115200);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // If you want to set the aref to something other than 5v
  analogReference(EXTERNAL);

  lcd.setBacklight(BLACK);
  
  minTemp = (((analogRead(tempPin) * aref_voltage)/1024.0) - 0.5)*100;
  maxTemp = minTemp;
}

float getTemp() {
  int tempReading;
  float voltage;
  float temperatureC;
  int i;
  
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

  return temperatureC;
}

void showTemp(float t1, float t2, float t3) {
  // Turn the LCD on
  lcd.setBacklight(WHITE);    // Set display colour to white
  lcd.display();

  // Display the current temp on the first line
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(t1);
  lcd.print(" C");
  
  // Display the temp range on the second line
  lcd.setCursor(0, 1);
  lcd.print("Rng: ");
  lcd.print(t2,1);
  lcd.print("-");
  lcd.print(t3,1);
}

void loop(void) {
  unsigned long currMillis;
  
  if (buttons = lcd.readButtons()) {
    if (buttons & BUTTON_SELECT) {
      currTemp = getTemp();
      if (currTemp < minTemp) minTemp = currTemp;
      if (currTemp > maxTemp) maxTemp = currTemp;
      showTemp(currTemp, minTemp, maxTemp);
    }
  }
  currMillis = millis();      
  if ((currMillis - prevMillis) > dispTime) {
    prevMillis = currMillis;
    lcd.noDisplay();            // Turn the display off
    lcd.setBacklight(BLACK);    // turn the backlight off 
  }
}

