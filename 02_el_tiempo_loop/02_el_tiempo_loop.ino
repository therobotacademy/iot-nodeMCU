// Esta es la librería para utilizar las funciones de red del ESP8266
#include <ESP8266WiFi.h> 

const char* ssid = "IO"; // Rellena con el nombre de tu red WiFi
const char* password = "FrutaFriaFrigorizada"; // Rellena con la contraseña de tu red WiFi

const char* host = "api.wunderground.com";
const char* apiKey = "769241281db33909"; // Puedes obtenerla en
                                          // https://www.wunderground.com/weather/api/

void setup() {
  Serial.begin(115200);
  delay(10);

  // Conectamos a la red WiFi

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  /* Configuramos el ESP8266 como cliente WiFi. Si no lo hacemos 
     se configurará como cliente y punto de acceso al mismo tiempo */
  WiFi.mode(WIFI_STA); // Modo cliente WiFi
  WiFi.begin(ssid, password);

  // Esperamos a que estemos conectados a la red WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected"); 
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); // Mostramos la IP
}

void loop() {
  Serial.print("TIMESTAMP at next iteration---------------   ");
  Serial.println(millis());
  Serial.print("connecting to ");
  Serial.println(host);
 
  // Creamos el cliente
  WiFiClient client;  // WiFiClientSecure client; si el servidor es https
  const int httpPort = 80; // Puerto HTTP
  if (!client.connect(host, httpPort)) {
    // ¿hay algún error al conectar?
    Serial.println("Ha fallado la conexión");
    return;
  }
 
  // Creamos la URL para la petición
  String url = "/api/";
  url += apiKey;
  //url += "/conditions/lang:SP/q/autoip.json";
  url += "/conditions/q/ES/sevilla.json";
  
 
  Serial.print("URL de la petición: http://");
  Serial.print(host);
  Serial.print(":");
  Serial.print(httpPort);
  Serial.println(url);
 
  // Enviamos la petición
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" + 
         "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Superado el tiempo de espera !");
      client.stop();
      return;
    }
  }

  // Consultar la memoria libre
  // Quedan un poco más de 40 kB
  // Utilizando HTTPS se necesitan unos 20 kB de memoria libres adicionales
  Serial.printf("\nMemoria libre en el ESP8266: %d Bytes\n\n",ESP.getFreeHeap());
 
  // Leemos la respuesta y la enviamos al monitor serie
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
 
  Serial.print("TIMESTAMP at end of iteration---------------   ");
  Serial.println(millis());
  delay(10000);
}
