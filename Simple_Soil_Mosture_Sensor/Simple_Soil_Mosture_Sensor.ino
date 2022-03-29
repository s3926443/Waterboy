/* Define variables */
int sensorPin = A0;
int sensorValue = 0;
int percent = 0;

const int dry = 1; // Value of the sensor when it is dry
const int wet = 697; // Value of the sensor when it is wet

/* Initialises Serial Communication */
void setup() { 
  Serial.begin(9600); 
} 

void loop() { 

  /* Read the sensor into a variable */
  sensorValue = analogRead(sensorPin);

  /* Check if the sensor has been disconnected or not */
  if (sensorValue == 0)
  {
    Serial.print("Sensor Disconnected\n");
  }
  else 
  {
      /* Convert the analog sensor into a percent value and print every 5 seconds */
      percent = convertToPercent(sensorValue);
      printValuesToSerial();
      delay(1000);
    } 
}

/* Function to convert analog value to a percent value and return */
int convertToPercent(int value)
{
  int percentValue = 0;
  percentValue = map(value, wet, dry, 100, 0);
  return percentValue;
}

/* Function to print the current analog and percent values to the serial monitor */
void printValuesToSerial()
{
  Serial.print("\n\nAnalog Value: ");
  Serial.print(sensorValue);
  Serial.print("\nPercent: ");
  Serial.print(percent);
  Serial.print("%");
}
