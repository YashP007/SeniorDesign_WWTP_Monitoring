// Blink-debugger.ino
// Read a pin number (0–10) over Serial, blink that pin until "quit" is received.

const unsigned long BLINK_INTERVAL = 500;  // milliseconds

int targetPin = D10;       // the pin we will blink
bool blinking = false;     // are we currently blinking?

void setup() {
  Serial.begin(115200);
  while(!Serial) ;        // wait for Serial monitor to open
  Serial.println(F("Enter a pin number 0–10, then press ENTER."));
  Serial.println(F("Type \"quit\" to stop blinking."));
}

void loop() {
  // If not blinking, try to read a command from Serial
  if (!blinking && Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.equalsIgnoreCase("quit")) {
      Serial.println(F("Nothing to quit; no pin is blinking."));
    }
    else {
      int p = cmd.toInt();
      if (p >= 0 && p <= 10 && cmd == String(p)) {
        targetPin = p;
        pinMode(targetPin, OUTPUT);
        Serial.print(F("Blinking pin "));
        Serial.println(targetPin);
        blinking = true;
      } else {
        Serial.print(F("Invalid pin: \""));
        Serial.print(cmd);
        Serial.println(F("\".  Enter 0–10 or \"quit\"."));
      }
    }
  }

  // If blinking, perform one on/off cycle, checking for "quit"
  if (blinking) {
    // Turn on
    digitalWrite(targetPin, HIGH);
    if (checkForQuit()) return;
    delay(BLINK_INTERVAL);

    // Turn off
    digitalWrite(targetPin, LOW);
    if (checkForQuit()) return;
    delay(BLINK_INTERVAL);
  }
}

// returns true if we saw "quit" and stopped blinking
bool checkForQuit() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.equalsIgnoreCase("quit")) {
      Serial.print(F("Stopped blinking pin "));
      Serial.println(targetPin);
      blinking = false;
      digitalWrite(targetPin, LOW);
      return true;
    }
    // if user types another number while blinking, restart on new pin
    int p = cmd.toInt();
    if (p >= 0 && p <= 10 && cmd == String(p)) {
      Serial.print(F("Switching to pin "));
      Serial.println(p);
      digitalWrite(targetPin, LOW);
      targetPin = p;
      pinMode(targetPin, OUTPUT);
      // continue blinking on new pin
      return false;
    }
    // otherwise ignore
  }
  return false;
}
