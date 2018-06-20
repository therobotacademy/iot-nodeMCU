/*
 Lectura de datos de temperatura y humedad con sensor DHT22 y placa 
 NodeMCU utilizando la función ESP.deepSleep() y monitorización en instancia propia
 de la plataforma www.thinger.io

 Ciclos con período de 10 segundos
   - El tiempo que está el microprocesador en reposo es de 7*10^6 us = 7 segundos.
   - El tiempo que se da para realizar la conexión WiFi, conexión con Thinger.io, realizar
 las medidas de temperatura y humedad y enviar los datos a la API es de 3000 ms = 3 segundos
 */

/***************************** CABECERA ***********************************************/
#define THINGER_SERVER "iot.therobotacademy.com"
#define _DEBUG_  // Lo usamos si tenemos problemas de conexión.
                   // La depuración específica la hacemos desde el sketch

#include <ESP8266WiFi.h>       //Librería de conexión WiFi del módulo ESP8266
#include <ThingerESP8266.h>    //Librería de la plataforma thinger.io
#include "DHT.h"               //Librería de los sensores DHT11, DHT22, etc. 

/***************************** VARIABLES Y CONSTANTES *********************************/
// Parámetros del DHT 
#define DHT11pin   D1       //Pin de conexión - GPIO5
#define DHT22pin   D2     // what digital pin we're connected to GPIO4 (= 4)

#define DHT11type DHT11
#define DHT22type DHT22   // DHT 22  (AM2302), AM2321

DHT dht11(DHT11pin, DHT11type);
DHT dht22(DHT22pin, DHT22type);

float h11, h22;      //Variables de humedad
float t11, t22;      //Variables de temperatura
boolean lectura_t11, lectura_h11;
boolean lectura_t22, lectura_h22;
int n_lectura = 1; 
long start_time; // Tiempo al inicio del último intervalo

//************** 70% en DEEP_SLEEP, ciclo de 10 s *****************               
#define READ_TIME  1000   // Tiempo máximo de lectura (+ aprox. 2 segundos que se tarda en la conexión)                         
#define SLEEP_TIME 7000  // Tiempo de Deep Sleep del ESP8266 (multiplicaremos por 1000 para pasar el dato en microsegundos)

/**************************** CONECTIVIDAD CON LA PLATAFORMA ********************************/
// THINGER.IO datos para conexión
#define user "brjapon"
#define device_Id "ESP8266_STREAM"
#define device_credentials "OSHD32vOevAZ"
ThingerESP8266 thing(user, device_Id, device_credentials);

// Datos de la red wifi a la que se conecta el dispositivo
const char* ssid = "IO";
const char* password = "FrutaFriaFrigorizada";
/******************************************************************************************/

void setup() {
  Serial.begin(115200);
 // Inicialización del sensor DHT  
  dht11.begin();
  dht22.begin();

  // Inicialización de la WiFi para comunicarse con la API
  thing.add_wifi(ssid, password);

  // Recursos que permiten la lectura de datos desde la API
  thing["DHT1122"] >> [](pson& out){
    out["DHT11_temperature"] = t11;
    out["DHT11_Humidity"] = h11;
    out["DHT22_temperature"] = t22;
    out["DHT22_Humidity"] = h22;
    };
  }

void loop() {
  thing.handle();
  start_time = millis();
  n_lectura = 1;  // Contador del número de intentos hasta que se consigue un dato válido
  lectura_t11 = true;  // Flag para registrar que el dato de TEMPERATURA es válido (sensor DHT11)
  lectura_h11 = true;  // Flag para registrar que el dato de HUMEDAD es válido (sensor DHT11)
  lectura_t22 = true;  // Flag para registrar que el dato de TEMPERATURA es válido (sensor DHT22)
  lectura_h22 = true;  // Flag para registrar que el dato de HUMEDAD es válido (sensor DHT22)
  
  // Tomamos la lectura del sensor ===============================================
  float t11_NOverificada, h11_NOverificada;
  float t22_NOverificada, h22_NOverificada;

  do {
    t11_NOverificada = dht11.readTemperature();
    h11_NOverificada = dht11.readHumidity();
    t22_NOverificada = dht22.readTemperature();
    h22_NOverificada = dht22.readHumidity();

    if (!isnan(t11_NOverificada)) {  // Si la lectura de TEMPERATURA contiene un valor numérico
      t11 = t11_NOverificada;  // Lectura correcta de temperatura
      lectura_t11 = true;
    }
    else
      lectura_t11 = false;   // no hemos recibido un número
    
    if (!isnan(h11_NOverificada)) {  // Si la lectura de HUMEDAD contiene un valor numérico
      h11 = h11_NOverificada;  // Lectura correcta de humedad
      lectura_h11 = true;
    }
    else
      lectura_h11 = false;   // no hemos recibido un número

    if (!isnan(t22_NOverificada)) {
      t22 = t22_NOverificada;
      lectura_t22 = true;
    }
    else
      lectura_t22 = false;
    
    if (!isnan(h22_NOverificada)) {  // Si la lectura de HUMEDAD contiene un valor numérico
      h22 = h22_NOverificada;
      lectura_h22 = true;
    }
    else
      lectura_h22 = false;
 
    // Debug específico de las lecturas (en bucle de lectura) =========================
    Serial.print("\n* Intento de lectura ");
    Serial.print(n_lectura);
    Serial.print(" en el instante (ms) ");
    Serial.print(millis()- start_time);
    
    Serial.print("\n temperatura DHT11 = ");
    if (lectura_t11)            Serial.print(t11);
    else                      Serial.print("err");
    Serial.print("\t humedad DHT11 = ");
    if (lectura_h11)            Serial.println(h11);
    else                      Serial.println("err");

    Serial.print("\n temperatura DHT22 = ");
    if (lectura_t22)            Serial.print(t22);
    else                      Serial.print("err");
    Serial.print("\t humedad DHT22 = ");
    if (lectura_h22)            Serial.println(h22);
    else                      Serial.println("err");
    // ================================================================================
    if (!lectura_t11 || !lectura_h11 || !lectura_t22 || !lectura_h22)   delay(1000);  // Espera para dar tiempo al hardware del sensor 
    n_lectura++;
  } while (isnan(t11_NOverificada) || isnan(h11_NOverificada) || isnan(t22_NOverificada) || isnan(h22_NOverificada));

  Serial.println("... Intento de escritura ...");
  thing.write_bucket("Stream_DHT11_DHT22", "DHT1122"); // Data bucket configurado como Data Source:”From Write Call”

  // Debug específico de las lecturas (tiempo de espera) ==============================
  Serial.print(millis()- start_time);
  Serial.println(" milisegundos para obtener medida");
  Serial.print("\n Suspendido (ms) hasta completar tiempo activo ...");
  Serial.println(READ_TIME -(millis()- start_time));
   
  // ===================================================================================   
  while ((millis()- start_time) < READ_TIME){ 
  delay(10);
  }
  
  // Debug específico de las lecturas (tiempo de SLEEP) ===============================
  Serial.print("\n.................NodeMCU durmiendo ..................");
  Serial.println(SLEEP_TIME);
      
  ESP.deepSleep(SLEEP_TIME*1000, WAKE_RF_DEFAULT);
     
  Serial.println("\n==================== Segunda lectura =========================");
}
