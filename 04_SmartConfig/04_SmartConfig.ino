#include <ESP8266WiFi.h>;

#define LED D4            // GPIO2 (D4), it is also the built in led

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.begin(115200);
  delay(10);

  WiFi.mode(WIFI_AP_STA);
  delay(2000);
  Serial.println(".Hello.");
  WiFi.beginSmartConfig();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.println(WiFi.smartConfigDone());
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(2000);
  Serial.println(WiFi.status());
  Serial.println(WiFi.SSID());
  if (WiFi.status() == WL_CONNECTED)
    digitalWrite(LED, HIGH);
  else digitalWrite(LED, LOW);
}
