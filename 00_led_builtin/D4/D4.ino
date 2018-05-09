#define LED_BLINK D4

void setup() {
  pinMode(LED_BLINK, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BLINK, LOW);   // Turn the LED on (Note that LOW is the voltage level)
  delay(1000);                      // Wait for a second

  digitalWrite(LED_BLINK, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(2000);                      // Wait for two seconds (to demonstrate the active low LED)
}
