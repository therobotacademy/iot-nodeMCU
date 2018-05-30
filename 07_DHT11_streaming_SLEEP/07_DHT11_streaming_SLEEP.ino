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
#define DHTPIN D1       //Pin de conexión - GPIO5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float h;      //Variable de humedad
float t;      //Variable de temperatura
boolean lectura_t, lectura_h;
int n_lectura = 1; 
long start_time; // Tiempo al inicio del último intervalo

//************** 70% en DEEP_SLEEP, ciclo de 10 s *****************
                  
#define READ_TIME  1000   // Tiempo máximo de lectura (+ aprox. 2 segundos que se tarda en la conexión)                         
#define SLEEP_TIME 7000  // Tiempo de Deep Sleep del ESP8266 (multiplicaremos por 1000 para pasar el dato en microsegundos)

/************** 70% en DEEP_SLEEP, ciclo de 150 s *****************
// Tiempo máximo de lectura
#define READ_TIME  43000 // + aprox. 2 segundos que se tarda en la conexión
// Tiempo de Deep Sleep del ESP8266 (en microsegundos)
#define SLEEP_TIME 105e6
*/
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
  dht.begin();

  // Inicialización de la WiFi para comunicarse con la API
  thing.add_wifi(ssid, password);

  // Recurso que permite la lectura de datos desde la API
  thing["DHT11"] >> [](pson& out){
    out["DHT11_temperature"] = t;
    out["DHT11_Humidity"] = h;
    };
  }

void loop() {
  thing.handle();
  start_time = millis();
  n_lectura = 1;  // Contador del número de intentos hasta que se consigue un dato válido
  lectura_t = true;  // Flag para registrar que el dato de TEMPERATURA es válido
  lectura_h = true;  // Flag para registrar que el dato de HUMEDAD es válido
  
  // Tomamos la lectura del sensor ===============================================
  float t_NOverificada, h_NOverificada;

  do {
    t_NOverificada = dht.readTemperature();
    h_NOverificada = dht.readHumidity();

    if (!isnan(t_NOverificada)) {  // Si la lectura de TEMPERATURA contiene un valor numérico
      t = t_NOverificada;  // Lectura correcta de temperatura
      lectura_t = true;
    }
    else
      lectura_t = false;   // no hemos recibido un número
    
    if (!isnan(h_NOverificada)) {  // Si la lectura de HUMEDAD contiene un valor numérico
      h = h_NOverificada;  // Lectura correcta de humedad
      lectura_h = true;
    }
    else
      lectura_h = false;   // no hemos recibido un número
 
    // Debug específico de las lecturas (en bucle de lectura) =========================
    Serial.print("\n* Intento de lectura ");
    Serial.print(n_lectura);
    Serial.print(" en el instante (ms) ");
    Serial.print(millis()- start_time);
    Serial.print("\t temperatura = ");
    if (lectura_t)            Serial.print(t);
    else                      Serial.print("err");
    Serial.print("\t humedad = ");
    if (lectura_h)            Serial.println(h);
    else                      Serial.println("err");
    // ================================================================================
    if (!lectura_t || !lectura_h)   delay(200);  // Espera para dar tiempo al hardware del sensor 
    n_lectura++;
  } while (isnan(t_NOverificada) || isnan(h_NOverificada));

  Serial.println("... Intento de escritura ...");
  thing.write_bucket("Stream_DHT11", "DHT11"); // Data bucket configurado como Data Source:”From Write Call”
  
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
