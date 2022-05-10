// C++ code

#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
// Viktor's DHT sensor library
//#include <DFRobot_DHT11.h>

// Grant's DHT11 sensor library
#include <DHT.h>

// RTC ////////////////////////////////
RTC_DS1307 rtc;

// Humidity Sensor ////////////////////////////////
// Viktor's DHT sensor
//DFRobot_DHT11 DHT;
//#define DHT11_PIN 3

// Grant's DHT sensor
#define pDHT 1
#define DHTTYPE DHT11
DHT dht( pDHT, DHTTYPE );

// moisture sensor ////////////////////////////////////////////////////////////////////////////////////////////////
int sensorPin = A0;
int sensorValue = 0;
int percent = 0;

// LEDs ///////////////////////////////////////////////////////////////////////////////////////////////////////////
int LEDRed = 2;
int LEDGreen = 3;

// Solenoid ///////////////////////////////////////////////////////////////////////////////////////////////////////
int solenoidPin = 13;

// LCD ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 11, en = 12, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup() {
  
  
  // rtc
  while (!Serial);
  Serial.begin(9600);
  
  // Grant's DHT11
  dht.begin();
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // Sets the time on the rtc to the time this code was compiled. only needed once,
    // can delete after running first time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("WATER-BOY!");

  pinMode(LEDRed, OUTPUT);
  pinMode(LEDGreen, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(solenoidPin, OUTPUT);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// set variables
bool isTesting = true;
bool isRunning = false;
bool isDisplaying = false;
int maxWatering = 300; //set time in seconds
int minSoilDryness = 30;
int maxSoilDryness = 90;
const int dry = 1; // Value of the sensor when it is dry
const int wet = 697; // Value of the sensor when it is wet
float humi = 0;
float temp = 0;
bool redIsSet = false;
bool greenIsSet = false;

int lastWateringDay;
// set day of the week by int; 0 = sun
int today;
int hour;
int mins;
uint32_t lastCheck = 0;
uint32_t lastStart = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t TimeSince(uint32_t timer, uint32_t current) {
  return current - timer;
}
uint32_t TimeRemaining(uint32_t timer, uint32_t current) {
  if (current >= timer) {
    return 0;
  }
  return timer - current;
}
void CloseValve() {
  // run function to close solanoid
  // include delay to allow it time to close
  // set isRunning
  isRunning = false;
  // set LED to green
  updateLED(true);

  digitalWrite(solenoidPin, LOW);       //Switch Solenoid OFF
  delay(1000);                          //Wait 1 Second
  Serial.print("\nClose Valve");
}

void OpenValve(uint32_t currentTime) {
  // run function to open solanoid
  // include delay to allow it time to open
  // set isRunning
  isRunning = true;
  lastStart = currentTime + maxWatering;
  lastWateringDay = today;
  // set LED to red
  updateLED(false);

  digitalWrite(solenoidPin, HIGH);      //Switch Solenoid ON
  delay(1000);                          //Wait 1 Second
  Serial.print("\nOpen Valve");
}

void DisplayOff() {
  // Turn off the display:
  lcd.noDisplay();
  //Serial.print("\n Display off");
}
void DisplayOn() {
  // Turn on the display:
  lcd.display();
  //Serial.print("\n Display on");
}

bool CheckWateringDay() {
  // add function to check day of the week.
  if (today != lastWateringDay)
  {
    // check if day is included in scheduled days
    // default true for testing
    return true;

  }
  return false;
}

bool CheckWateringWindow() {
  // add function to check a time of day to water.
  // return true by default
 if (hour == 6)
  {
    return true;
  }
  return false;
}

bool CheckDrySoilMin(int value) {
  // Compare % TO MIN MOISTURE level and decide if it meets requirement
  if (value != 0)
  {
    if (value <= minSoilDryness) {
      return true;
    }
  }

  return false;
}
bool CheckDrySoilMax(int value) {
  // Compare % TO MIN MOISTURE level and decide if it meets requirement
  if (value != 0)
  {
    if (value > maxSoilDryness) {
      return true;
    }
  }

  return false;
}
int GetMoisturePercentage(int value) {
  int percentValue = 0;
  percentValue = map(value, wet, dry, 100, 0);
  return percentValue;
}

int CheckTemp() {
  if (temp >= 28) {

    return true;
  }
  return false;
}

/* Function to print the current analog and percent values to the serial monitor */
void printValuesToSerial()
{
  if (sensorValue != 0) {
    Serial.print("\n\nMoisture Analog Value: ");
    Serial.print(sensorValue);
    Serial.print("\nMoisture Percent: ");
    Serial.print(percent);
    Serial.print("%");
  }

  if (temp != 0) {
    Serial.print("\n\nTemperature: ");
    Serial.print(temp);
    Serial.print("°C  ~ ");
    Serial.print(" Humidity: ");
    Serial.print(humi);
    Serial.println("%");
  }
}

/* Function to convert analog value to a percent value and return */
int convertToPercent(int value)
{
  int percentValue = 0;
  percentValue = map(value, wet, dry, 100, 0);
  return percentValue;
}

void setLEDGreen() 
{
  if (not greenIsSet) {
    greenIsSet = true;
    redIsSet = false;
    digitalWrite(LEDGreen, HIGH);
    digitalWrite(LEDRed, LOW);
    Serial.print("\nSet LED Green");
  }
}

void setLEDRed() 
{
  if (not redIsSet) {
    redIsSet = true;
    greenIsSet = false;
    digitalWrite(LEDRed, HIGH);
    digitalWrite(LEDGreen, LOW);
    Serial.print("\nSet LED Red");
  }
}
void updateLED(bool value)
{
  if (value == true) {
    setLEDGreen();
  }
  else {
    setLEDRed();
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

   // Grant's DHT11
  float fHumidity = dht.readHumidity();
  float fTemperature = dht.readTemperature();
  if ( isnan( fTemperature ) || isnan( fHumidity ) ) {
    Serial.print( "\n" );
    Serial.println( "Failed to read from DHT" );
  } else {
    Serial.print( fHumidity );
    Serial.print(",");
    Serial.println( fTemperature );
  } 
  delay( 3000 );
  
  const DateTime now = rtc.now();
  int today = now.dayOfTheWeek();
  int hour = now.hour();
  int mins = now.minute();
  uint32_t epoch = now.unixtime();
  
  //Viktor DHT
  // float humi = DHT.humidity;
  //float temp = DHT.temperature;
  // DHT.read(DHT11_PIN);
  
  if (isTesting) {
   isRunning = true;
   maxWatering = 10;
   lastStart = epoch + maxWatering;
   maxSoilDryness = 200;
   hour = 6;
   setLEDRed();
   OpenValve(epoch);
   isTesting = false;
  }
  /* Read the sensor into a variable */
  sensorValue = analogRead(sensorPin);
  // clear percent value each pass to make sure its not using bad data if sensor gets disconected
  percent = 0;
  /* Check if the sensor has been disconnected or not */
  if (sensorValue == 0)
  {
    Serial.print("Sensor Disconnected\n");
  }
  else
  {
    /* Convert the analog sensor into a percent value and print every 5 seconds */
    percent = convertToPercent(sensorValue);
  }

  printValuesToSerial();

  lcd.clear();
  if (isRunning) {
    uint32_t remaining = TimeRemaining(lastStart, epoch);

    lcd.setCursor(0, 0);
    lcd.print("Watering for ");
    lcd.setCursor(0, 1);
    lcd.print(remaining);
    lcd.print(" seconds");

    if (remaining <= 0 or (CheckDrySoilMax(percent))) {
      CloseValve();
      return;
    }
    else {
      return;
    }
  }

  // now we know the day we can check if the schedule was triggered today.
  uint32_t remaining = TimeRemaining(lastCheck, epoch);
  if (remaining <= 0) {
    if (CheckWateringDay() and not CheckDrySoilMax(percent)) {
      if (CheckWateringWindow()) {
        OpenValve(epoch);
        return;
      }
    }

    // check soil dryness, add a delay to rewatering as the water may not have filtered through yet.
    if (CheckDrySoilMin(percent)) {
      OpenValve(epoch);
      return;
    }

    // default delay 6 hours unless hot.
    uint32_t delayValue = (60 * 60 * 6);
    // check  time to delay next check
    // can we check todays forecast?
    if (CheckTemp()) {
      delayValue = (60 * 60 * 3);
    }
    if (hour > 18 or hour < 6) {
      int remainingHours = 0;
      if (hour > 18) {
        remainingHours = ((23-hour) + 5);

      }
      else {
        remainingHours = (5-hour);
      }
      int remainingMins = (63-mins);
      delayValue = ((remainingHours * 60 * 60) + remainingMins * 60);
    }
    lastCheck = (epoch + delayValue);
  }
  uint32_t outTime = remaining / 60;
  String length = " mins";
  if (outTime > 60) {
    outTime = outTime / 60;
    length = " hours";
  }
  else if (remaining < 60) {
    outTime = remaining;
    length = " secs";
  }
  
  // set LED to green
  updateLED(true);
  
  lcd.setCursor(0, 0);
  lcd.print("Next check in ");
  lcd.setCursor(0, 1);
  lcd.print(outTime);
  lcd.print(length);

  //digitalWrite(LED_BUILTIN, HIGH);
  //delay(1000); // Wait for 1000 millisecond(s)
  //digitalWrite(LED_BUILTIN, LOW);
}
