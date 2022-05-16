#include "TinyDHT.h"
#define DHTPIN 4  // DHT connected to Arduino Uno Digital Pin 2
#define DHTTYPE DHT11   // DHT 11 

DHT dht(DHTPIN, DHTTYPE);

int MoisturePin = A0;  // Soil Moisture sensor connected to Analog Pin A0
// int sensorValue = 0;  
// int percent = 0;

int dryValue = 0;   // Value for moisture sensor when soil is dry
int wetValue = 700;  // Value for moisture sensor when soil is wet
int friendlyDryValue = 0;
int friendlyWetValue = 100;

void setup() {
  Serial.begin(9600); // Output status on Uno serial monitor
  dht.begin();
}

void loop() {

 int rawValue = analogRead(MoisturePin);
  Serial.print("Raw Soil Moisture Reading: ");
  Serial.print(rawValue);
  Serial.print(" | ");
  
  int friendlyValue = map(rawValue, dryValue, wetValue, friendlyDryValue, friendlyWetValue);  //Calculates the percentage for display
  
  Serial.print("Soil Moisture: ");
  Serial.print(friendlyValue);
  Serial.println("%");

  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  int8_t h = dht.readHumidity();
  int16_t t = dht.readTemperature();

  // check if returns are valid then something went wrong!
  if ( t == BAD_TEMP || h == BAD_HUM ) { // if error conditions          
    Serial.println("Failed to read from DHT");
  } else {
    Serial.print("Air Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Air Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");
  }
  delay(2000);

}
