// C++ code
//
#include <time.h>

// moisture sensor
int sensorPin = A0;
int sensorValue = 0;
int percent = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// set variables
bool isRunning = false;
int maxWatering = 300; //set time in seconds
int minSoilDryness = 30;
int maxSoilDryness = 70;
// set day of the week by int; 0 = sun
int lastWateringDay;
int today;
int lastStart = 0;
int currentTime = 0;
const int dry = 1; // Value of the sensor when it is dry
const int wet = 697; // Value of the sensor when it is wet

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
int TimeSince(int timer){
  // check how to pull current time and subtract saved time from it to get time since.
  //return Now() - timer;
  return 0;
}
void CloseValve() {
  // run function to close solanoid
  // include delay to allow it time to close
  // set isRunning
  isRunning = false;
  delay(300000); // Wait for 5 mins to allow water to filter down before watering by moisture level
}

void OpenValve() {
  // run function to open solanoid
  // include delay to allow it time to open
  // set isRunning
  isRunning = true;
  lastWateringDay = today;
  delay(3000); // Wait for 3000 millisecond(s)
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
  return true;
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
int GetMoisturePercentage(int value)
{
  int percentValue = 0;
  percentValue = map(value, wet, dry, 100, 0);
  return percentValue;
}

int CheckTemp() {
  // add code to check temp
  // can we check weather forcast for the day?
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  
    
  if (isRunning)
  {
    if (TimeSince(lastStart) >= maxWatering or (CheckDrySoilMax()))
    {
      CloseValve();
      return;
    }
  	else 
    {
      // run delay to loop and  end loop as we have nothing to check
      return;
    }
  } 
  
  // now = epoch time
  time_t now = time(0);
  tm *ltm = localtime(&now);
  
  // tm_mday; this should return the day of the month by number eg: 9 for 9th april
  // tm_wday; this will return day of the week in an int; 0 = SUN 6 = SAT
  today = ltm->tm_wday;
  // now we know the day we can check if the schedule was triggered today.
  
  if (CheckWateringDay() and not CheckDrySoilMax())
  {
    if (CheckWateringWindow())
    {
      OpenValve();
      return;
    }
  }
  
  // check soil dryness, add a delay to rewatering as the water may not have filtered through yet.
  if (CheckDrySoilMin())
  {
    OpenValve();
    return;
  }
  
  int delayValue = 1000;
  // check  time to delay next check
  // can we check todays forecast?
  if (CheckTemp())
  {
    delayValue = 21600000; // 1000 ms = 1 second * 60 seconds * 60 mins * 6 hours
  }
  else
  {
     // push timer to check 4 am tomorrow?
  }
  
  
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
  digitalWrite(LED_BUILTIN, LOW);
  delay(delayValue);
}