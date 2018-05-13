// Including the ESP8266 WiFi library
#include <ESP8266WiFi.h>
#include "DHT.h"
// Including the Thinger.io library
#include <ThingerESP8266.h>

// Uncomment one of the lines below for whatever DHT sensor type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22     // DHT 22  (AM2302), AM2321
#define LED D4            // GPIO2 (D4), it is also the built in led

// DHT Sensor
const int DHTPin = D2;     // what digital pin we're connected to GPIO4 (= 4)
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// Replace with your network details
const char* ssid = "IO";
const char* password = "FrutaFriaFrigorizada";
WiFiClient client;

/******************************************************************************************/
// ThingSpeak information
char thingSpeakAddress[] = "api.thingspeak.com";
unsigned long channelID = 12874; // Numero de canal en Thingspeak
// NOT USED // char* readAPIKey = "XXXXXX"; //Clave lectura. 
char* writeAPIKey = "X7EGQRWP7DR5SS6K"; // Clave escritura. 
const unsigned long postingInterval = 300L * 1000L; //Cada 5 minutos
const unsigned long IntervaloParpadeoLed = 1L * 1000L; //Cada 1 segundo
unsigned int dataFieldOne = 1;                            // Field to write temperature data
unsigned int dataFieldTwo = 2;                       // Field to write temperature data
unsigned int dataFieldThree = 3;                     // Field to write elapsed time data
/******************************************************************************************/

// Global variables for ThingSpeak
unsigned long lastConnectionTime = 0;
long lastUpdateTime = 0; 
long tParpadeoLed=0;
boolean on_off = LOW;


// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
 
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Connecting to Wifi and ThingSpeak ////////////////
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  ThingSpeak.begin(client);
  Serial.println("Web client running. Waiting for the ESP IP...");
  delay(2000);
    // Printing the ESP IP address
  Serial.println(WiFi.localIP());
  /////////////////////////////////////////////////////
  Serial.println("Start");
  dht.begin();

  Serial.println("Setup");
  Serial.print("Milisegundos desde arranque (ms) ");
  Serial.println(millis());
  Serial.print("Tiempo desde la ultima vez (ms) ");
  Serial.println(lastUpdateTime);
  Serial.print("Intervalo entre envio datos (ms) ");
  Serial.println(postingInterval);

  // First set of data
  float minutes = millis()/1000.0/60.0;
  float temperature = dht.readTemperature(); // Read temperature as Celsius (the default). Use 'true' argument for Farenheit
  float humidity = dht.readHumidity(); // Read humidity 
  write2TSData( channelID , dataFieldOne , temperature , dataFieldTwo , humidity , dataFieldThree , minutes );
}

// runs over and over again
void loop() {
    //Parpadea el led de placa cada tParpadeoLed
    if (millis() - tParpadeoLed >=  IntervaloParpadeoLed) {
         tParpadeoLed = millis();
        //Conmuta el led
        on_off = !on_off;
        digitalWrite(LED, on_off);
    }
    
    // Only update if posting time is exceeded
    if (millis() - lastUpdateTime >=  postingInterval) {
        lastUpdateTime = millis();
        float minutes = lastUpdateTime/1000.0/60.0;
        float temperature = dht.readTemperature(); // Read temperature as Celsius (the default). Use 'true' argument for Farenheit
        float humidity = dht.readHumidity(); // Read humidity    
        // Check if any reads failed and exit early (to try again).
        if (isnan(humidity) || isnan(temperature)) {
          Serial.println("Failed to read from DHT sensor!");
        }
        Serial.println("");
        Serial.print("Sample OK at ");
        Serial.print(minutes,2); Serial.print(" minutes\t"); 
        Serial.print(temperature,1); Serial.print(" ºC, ");
        Serial.print((int)humidity); Serial.println(" %");
        // Write to ThingSpeak channels: THIS FUNCTIONS FREEZE THE EXECUTION UNTIL THE NEXT ITERATION
        write2TSData( channelID , dataFieldOne , temperature , dataFieldTwo , humidity , dataFieldThree , minutes );      // Write the temperature in F, C, and time since starting.  
    }
}

// Function definition
int writeTSData( unsigned long TSChannel, unsigned int TSField, float data ){
  int  writeSuccess = ThingSpeak.writeField( TSChannel, TSField, data, writeAPIKey ); // Write the data to the channel
  if ( writeSuccess ){
    
    Serial.println( String(data) + " written to Thingspeak." );
    }    
    return writeSuccess;
}

//use this function if you want multiple fields simultaneously
int write2TSData( unsigned long TSChannel, unsigned int TSField1, float field1Data, unsigned int TSField2, long field2Data, unsigned int TSField3, long field3Data ){

  ThingSpeak.setField( TSField1, field1Data );
  ThingSpeak.setField( TSField2, field2Data );
  ThingSpeak.setField( TSField3, field3Data );
   
  int writeSuccess = ThingSpeak.writeFields( TSChannel, writeAPIKey );
  return writeSuccess;
}

