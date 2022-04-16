// C++ code
//#define BLYNK_PRINT Serial
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal.h>
//#include <ESP8266_Lib.h>
//#include <BlynkSimpleShieldEsp8266.h>
RTC_DS1307 rtc;

// moisture sensor ////////////////////////////////////////////////////////////////////////////////////////////////
int sensorPin = A0;
int sensorValue = 0;
int percent = 0;

// LCD ////////////////////////////////////////////////////////////////////////////////////////////////////////////

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 11, en = 12, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Blynk //////////////////////////////////////////////////////////////////////////////////////////////////////////
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "insert blynk auth code";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "insert ssid";
char pass[] = "insert password";

// Hardware Serial on Mega, Leonardo, Micro...
#define EspSerial Serial1

// or Software Serial on Uno, Nano...
//#include <SoftwareSerial.h>
//SoftwareSerial EspSerial(2, 3); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 115200

//ESP8266 wifi(&EspSerial);

// Attach virtual serial terminal to Virtual Pin V1
//WidgetTerminal terminal(V1);

// You can send commands from Terminal to your hardware. Just use
// the same Virtual Pin as your Terminal Widget
/*BLYNK_WRITE(V1)
{

  // if you type "Marco" into Terminal Widget - it will respond: "Polo:"
  if (String("Marco") == param.asStr()) {
    terminal.println("You said: 'Marco'") ;
    terminal.println("I said: 'Polo'") ;
  } else {

    // Send it back
    terminal.print("You said:");
    terminal.write(param.getBuffer(), param.getLength());
    terminal.println();
  }

  // Ensure everything is sent
  terminal.flush();
}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // rtc
  while (!Serial);
  Serial.begin(57600);
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
  
  pinMode(LED_BUILTIN, OUTPUT);
  
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// set variables
int index = 0;
bool isRunning = false;
bool isDisplaying = false;
int maxWatering = 300; //set time in seconds
int minSoilDryness = 30;
int maxSoilDryness = 70;
const int dry = 1; // Value of the sensor when it is dry
const int wet = 697; // Value of the sensor when it is wet

int lastWateringDay;
// set day of the week by int; 0 = sun
int today;
int hour;
int mins;
uint32_t lastCheck = 0;
uint32_t lastStart = 0;

#define SUN                     0x01
#define MON                     0x02
#define TUE                     0x04
#define WED                     0x08
#define THR                     0x10
#define FRI                     0x20
#define SAT                     0x40
#define EVERYDAY                (SUN | MON | TUE | WED | THR | FRI | SAT)

char weekday[][4] = {"SUN","MON","TUE","WED","THU","FRI","SAT"};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t TimeSince(uint32_t timer, uint32_t current){
  return current - timer;
}
uint32_t TimeRemaining(uint32_t timer, uint32_t current){
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
  delay(3000); // Wait for 5 mins to allow water to filter down before watering by moisture level
}

void OpenValve(uint32_t currentTime) {
  // run function to open solanoid
  // include delay to allow it time to open
  // set isRunning
  isRunning = true;
  lastStart = currentTime + maxWatering;
  lastWateringDay = today;
  delay(3000); // Wait for 3000 millisecond(s)
}

void DisplayOff() {
  // Turn off the display:
  lcd.noDisplay();
  delay(500);
}
void DisplayOn() {
  // Turn on the display:
  lcd.display();
  delay(2000);
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

bool CheckDrySoilMin() {
  // Compare % TO MIN MOISTURE level and decide if it meets requirement
  //* Read the sensor into a variable */
  sensorValue = analogRead(sensorPin);
  if (sensorValue != 0)
  {
    if (GetMoisturePercentage(sensorValue) <= minSoilDryness) {
  	  return true;
    }
  }
  
  return false;
}
bool CheckDrySoilMax() {
  // Compare % TO MIN MOISTURE level and decide if it meets requirement
  //* Read the sensor into a variable */
  sensorValue = analogRead(sensorPin);
  if (sensorValue != 0)
  {
    if (GetMoisturePercentage(sensorValue) > maxSoilDryness) {
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
  // add code to check temp
  // can we check weather forcast for the day?
  return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
 
  const DateTime now = rtc.now();
  int today = now.dayOfTheWeek();
  int hour = now.hour();
  int mins = now.minute();
  uint32_t epoch = now.unixtime();
  
  if (isRunning) {
    uint32_t remaining = TimeRemaining(lastStart,epoch);
    lcd.clear();
    lcd.print("Watering for ");
    lcd.print(remaining);
    lcd.print(" seconds.");
    if (remaining <= 0 or (CheckDrySoilMax())) {
      CloseValve();
      return;
    }
  	else {
      // run delay to loop and  end loop as we have nothing to check
      return;
    }
  } 

  // now we know the day we can check if the schedule was triggered today.
  uint32_t remaining = TimeRemaining(lastCheck,epoch);
  if (remaining <= 0) {
    if (CheckWateringDay() and not CheckDrySoilMax()) {
      if (CheckWateringWindow()) {
        OpenValve(epoch);
        return;
      }
    }

    // check soil dryness, add a delay to rewatering as the water may not have filtered through yet.
    if (CheckDrySoilMin()) {
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
	  lastCheck = (epoch + delayValue);
  }
  
  lcd.clear();
  lcd.print("Idle for ");
  lcd.print(remaining / 60);
  lcd.print(" mins");

  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
  digitalWrite(LED_BUILTIN, LOW);