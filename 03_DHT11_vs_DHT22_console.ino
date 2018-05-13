#include "DHT.h"
 
// Uncomment whatever type you're using!
#define DHT11type DHT11   // DHT 11
#define DHT22type DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

 
const int DHT11Pin = D1;     // what digital pin we're connected to GPIO5 (= 5)
const int DHT22Pin = D2;     // what digital pin we're connected to GPIO4 (= 4)
 
DHT dht11(DHT11Pin, DHT11type);
DHT dht22(DHT22Pin, DHT22type);
 
void setup() {
   Serial.begin(115200);
//   Serial.println("DHT11 vs. DHT22 test and comparison!");
   dht11.begin();
   dht22.begin();
}
 
void loop() {
   // Wait a few seconds between measurements.
   delay(2000);
 
   // Reading temperature or humidity takes about 250 milliseconds!
   // Readings of DHT11
   float h11 = dht11.readHumidity();
   float t11 = dht11.readTemperature();
   // Readings of DHT22
   float h22 = dht22.readHumidity();
   float t22 = dht22.readTemperature();
 
   if (isnan(h11) || isnan(t11)) {
      Serial.println("Failed to read from DHT11 sensor!");
      // return;
   }
    if (isnan(h22) || isnan(t22)) {
      Serial.println("Failed to read from DHT22 sensor!");
      return;
   }
 
   Serial.print("DHT11 Humidity: ");
   Serial.print(h11);
   Serial.print(" %\t");
   Serial.print("Temperature: ");
   Serial.print(t11);
   Serial.print(" *C ---- ");
   
   Serial.print("DHT22 Humidity: ");
   Serial.print(h22);
   Serial.print(" %\t");
   Serial.print("Temperature: ");
   Serial.print(t22);
   Serial.println(" *C");
}
