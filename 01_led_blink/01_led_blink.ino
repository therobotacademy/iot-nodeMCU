// Nomenclatura NodeMCU
#define LED_BUILTIN D4 // GPIO2
#define BUTTON D6
// #define BUTTON D3 // GPIO0, if you want to use the FLASH button

/*/ Nomenclatura ESP8266
#define LED_BUILTIN 2 // GPIO2 (D4)
#define BUTTON_BUILTIN 0 // GPIO0 (D3)
*/
void setup() { 
  // El LED integrado está conectado al pin 2.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON, INPUT);
} 

void loop() {
  // Leer el estado del botón. Cuando está pulsado el pin se pone a nivel bajo 
  int estado = digitalRead(BUTTON); 
  // Configurar el LED de acuerdo al estado del botón 
  digitalWrite(LED_BUILTIN, estado); 
}
