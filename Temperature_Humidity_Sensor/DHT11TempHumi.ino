/*!
 * Waterboy DHT11 Temperature and Humidity Sensor
 */

#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT;
#define DHT11_PIN 4

void setup(){
  Serial.begin(9600);
}

void loop(){

  float humi = DHT.humidity;
  float temp = DHT.temperature;
  
  
  DHT.read(DHT11_PIN);
  if (isnan(humi) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("°C  ~ ");
  Serial.print(" Humidity: ");
  Serial.print(humi);
  Serial.println("%");
  delay(1000);
  }
}
