const int ledPin = 9;  // D9 = GPIO9 on XIAO ESP32-C3

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  Serial.println("Blinking LED on D9 (GPIO9)...");
}

void loop() {
  digitalWrite(ledPin, HIGH);   // Turn LED ON
  delay(500);                   // Wait 500 ms
  digitalWrite(ledPin, LOW);    // Turn LED OFF
  delay(500);                   // Wait 500 ms
}