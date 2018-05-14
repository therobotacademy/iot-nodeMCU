// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include "DHT.h"
// Including the Thinger.io library
#include <ThingerESP8266.h>

#define DHT11type DHT11   // DHT 11
#define DHT22type DHT22   // DHT 22  (AM2302), AM2321

// DHT Sensors
const int DHT11Pin = D1;     // what digital pin we're connected to GPIO5 (= 5)
const int DHT22Pin = D2;     // what digital pin we're connected to GPIO4 (= 4)

// Initialize DHT sensors for data acquisition ***** Local in NodeMCU
DHT dht11(DHT11Pin, DHT11type);
DHT dht22(DHT22Pin, DHT22type);

/******************************************************************************************/
// THINGER.IO connection information
#define user "brjapon"
#define device_Id "ESP8266"
#define device_credentials "OSHD32vOevAZ"
ThingerESP8266 thing(user, device_Id, device_credentials);

// Replace with your network details
const char* ssid = "IO";
const char* password = "FrutaFriaFrigorizada";
/******************************************************************************************/

// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  // Local objects in NodeMCU, one for each DHT type sensor
  dht11.begin();
  dht22.begin();

  // Cloud objects in Thinger.io
  thing.add_wifi(ssid, password);

  // Data acquisition initialization from Thinger.io API
  // DHT11 sensor
  thing["DHT11"] >> [](pson& out){   // This is the 'Device ID' in Thinger.io
    out["DHT11_temperature"] = dht11.readTemperature();
    out["DHT11_Humidity"] = dht11.readHumidity();
    };
  // DHT22 sensor
  thing["DHT22"] >> [](pson& out){
    out["DHT22_temperature"] = dht22.readTemperature();
    out["DHT22_Humidity"] = dht22.readHumidity();
    };

  // Just for debugging purposes
  float t11 = dht11.readTemperature();
  float h11 = dht11.readHumidity();
  Serial.print("\t t11 = ");
  Serial.print(t11);
  Serial.print("\t h11 = ");
  Serial.println(h11);

  float t22 = dht22.readTemperature();
  float h22 = dht22.readHumidity();
  Serial.print("\t t22 = ");
  Serial.print(t22);
  Serial.print("\t h22 = ");
  Serial.println(h22);
}

// runs over and over again
void loop() {
  thing.handle();
}


