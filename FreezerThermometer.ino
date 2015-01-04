/*
Freezer thermometer
 Uses an Analog Devices TMP36 temperature sensor and displays
 the current temperature on an Adafruit RGB LCD shield
 Resolution is 10 mV/deg C with a 500 mV offset to allow for negative temperatures
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
#define aref_voltage 3.3         // we tie 3.3V to ARef

byte tempPin = 1;    // analog pin for the TMP36 Vout pin
float currTemp = 0.0;
float minTemp = 0.0;
float maxTemp = 0.0;
float hiTempAlert = -10.0;
float lowTempAlert = -25.0;
// Create arrays for 5 timers
// timer[0] - time interval between reading the thermometer
// timer[1] - Unused
// timer[2] - Unused
// timer[3] - Unused
// timer[4] - Unused
unsigned long timer[5];
byte timerState[5];

// Get an LCD object
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup() {
  // Debugging output
  Serial.begin(115200);

  // If you want to set the aref to something other than 5v
  analogReference(EXTERNAL);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  lcd.setBacklight(BLACK);
  
  minTemp = (((analogRead(tempPin) * aref_voltage)/1024.0) - 0.5)*100;
  maxTemp = minTemp;
}

void loop(void) {  
  uint8_t buttons;
  int tempInterval = 1;    // Time interval between readings

  buttons = lcd.readButtons();
  if (buttons & BUTTON_SELECT) {
    // Activate the display and show the current temperature reading
    currTemp = getTemp();
    if (currTemp < minTemp) minTemp = currTemp;
    if (currTemp > maxTemp) maxTemp = currTemp;
    showTemp(currTemp, minTemp, maxTemp);
  }
  if (buttons & BUTTON_UP) {
    // Set high alert temp using the up/down buttons
    // Turn the LCD on
    lcd.setBacklight(WHITE);    // Set display colour to white
    lcd.display();
    lcd.setCursor(0,0);
    lcd.print("Hi Temp Alert");
    lcd.setCursor(0,1);
    lcd.print(hiTempAlert);    // Display the current hi temp alert
    lcd.print(" C");
    hiTempAlert = setAlert(hiTempAlert);
  }
  if (buttons & BUTTON_DOWN) {
    // Turn the LCD on
    lcd.setBacklight(WHITE);    // Set display colour to white
    lcd.display();
    // Set low alert temp using the up/down buttons
    lcd.setCursor(0,0);
    lcd.print("Low Temp Alert");
    lcd.setCursor(0,1);
    lcd.print(lowTempAlert);    // Display the current low temp alert
    lcd.print(" C");
    lowTempAlert = setAlert(lowTempAlert);
  }
  if (buttons & BUTTON_LEFT) {
    // Use left button to adjust temperature sampling interval
    lcd.setBacklight(WHITE);
    lcd.display();
    lcd.setCursor(0,0);
    lcd.print("Sample Interval");
    lcd.setCursor(0,1);
    lcd.print(tempInterval);
    lcd.print(" min");
    tempInterval = setInterval(tempInterval);
  }

  // Use Timer 0 for the temperature sampling interv
  if (delayMinutes(0,tempInterval)) {
    currTemp = getTemp();    // Get a temperature reading every tempInterval minutes
    if (currTemp < minTemp) minTemp = currTemp;
    if (currTemp > maxTemp) maxTemp = currTemp;
    if ((currTemp > hiTempAlert) || (currTemp < lowTempAlert)) {
      lcd.setBacklight(RED);
      showTemp(currTemp, minTemp, maxTemp);
    }
//    lcd.noDisplay();            // Turn the display off
//    lcd.setBacklight(BLACK);    // turn the backlight off 
  }
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
  
  return;
}

float setAlert(float alertTemp) {
  uint8_t buttons;

  buttons = lcd.readButtons();
  while (!(buttons & BUTTON_SELECT)) {
    if (buttons & BUTTON_UP) alertTemp++;
    if (buttons & BUTTON_DOWN) alertTemp--;
    lcd.setCursor(0,1);
    lcd.print(alertTemp);
    lcd.print(" C");
  }
  return alertTemp;
}

int setInterval(int interval) {
  uint8_t buttons;

  buttons = lcd.readButtons();
  while (!(buttons & BUTTON_SELECT)) {
    if (buttons & BUTTON_UP) interval++;
    if (buttons & BUTTON_DOWN) interval--;
    lcd.setCursor(0,1);
    lcd.print(interval);
    lcd.print(" min");
  }
  return interval;
}

/*  Delay without "delay()"
 *   Chad Higgins 2012
 *   Demonstates how to use write a delay that is like "blink without delay" but is used much like "delay()"
 * http://playground.arduino.cc/Code/DelaySeconds
 */
int delayHours(byte timerNumber,unsigned long delaytimeH){    
  //Here we make it easy to set a delay in Hours
  return delayMilliSeconds(timerNumber,delaytimeH*1000*60*60);
}
int delayMinutes(byte timerNumber,unsigned long delaytimeM){    
  //Here we make it easy to set a delay in Minutes
  return delayMilliSeconds(timerNumber,delaytimeM*1000*60);
}
int delaySeconds(byte timerNumber,unsigned long delaytimeS){    
  //Here we make it easy to set a delay in Seconds
  return delayMilliSeconds(timerNumber,delaytimeS*1000);
}

int delayMilliSeconds(int timerNumber,unsigned long delaytime){
  unsigned long timeTaken;
  if (timerState[timerNumber]==0){    
    //If the timer has been reset (which means timer (state ==0) then save millis() to the same number timer, 
    timer[timerNumber]=millis();
    timerState[timerNumber]=1;      //now we want mark this timer "not reset" so that next time through it doesn't get changed.
  }
  if (millis()> timer[timerNumber]){
    timeTaken=millis()+1-timer[timerNumber];    //here we see how much time has passed
  }
  else{
    timeTaken=millis()+2+(4294967295-timer[timerNumber]);    //if the timer rolled over (more than 48 days passed)then this line accounts for that
  }
  if (timeTaken>=delaytime) {
    //here we make it easy to wrap the code we want to time in an "IF" statement, if not then it isn't and so doesn't get run.
     timerState[timerNumber]=0;  //once enough time has passed the timer is marked reset.
     return true;                          //if enough time has passed the "IF" statement is true
  }
  else {                               
    //if enough time has not passed then the "if" statement will not be true.
    return false;
  }
}

