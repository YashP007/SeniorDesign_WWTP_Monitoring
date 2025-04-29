#include <Wire.h>

// Explicit I2C pin assignment for XIAO ESP32-C3
#define SDA_PIN 4+2   // D4 = GPIO4
#define SCL_PIN 5+2   // D5 = GPIO5

// LMP91000 MENB controls (disable all for scan)
const uint8_t MENB_PINS[] = { 0+2, 1+2, 2+2, 3+2 };
const uint8_t NUM_MENB = sizeof(MENB_PINS) / sizeof(MENB_PINS[0]);

// maximum addresses we can store
#define MAX_FOUND  32
sx
// function prototypes
int  scanI2C(uint8_t found[]);
void printScanResult(const uint8_t found[], int count);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("\nI²C Scanner with MENB sequencing\n");

  // initialize MENB pins to disable all devices
  for (uint8_t i = 0; i < NUM_MENB; i++) {
    pinMode(MENB_PINS[i], OUTPUT);
    digitalWrite(MENB_PINS[i], HIGH);
  }

  // init I2C
  Wire.begin(SDA_PIN, SCL_PIN, 100000);
  delay(200);
}

void loop() {
  uint8_t found[MAX_FOUND];
  int     nDevices;

  // 1) scan with all MENB HIGH
  Serial.println("=== Global scan (all MENB disabled) ===");
  nDevices = scanI2C(found);
  printScanResult(found, nDevices);

  // 2) scan with one MENB LOW at a time
  for (uint8_t d = 0; d < NUM_MENB; d++) {
    // disable all
    for (uint8_t i = 0; i < NUM_MENB; i++) {
      digitalWrite(MENB_PINS[i], HIGH);
    }
    // enable just device d
    digitalWrite(MENB_PINS[d], LOW);

    Serial.printf("\n--- Scan with MENB%u LOW (only device %u) ---\n", d, d);
    nDevices = scanI2C(found);
    printScanResult(found, nDevices);
  }

  Serial.println("\n>> cycle complete, waiting 15s\n");
  delay(15000);
}

// scan the full 0x03–0x77 range, fill found[], return count
int scanI2C(uint8_t found[]) {
  int count = 0;
  for (uint8_t addr = 0x03; addr <= 0x77; addr++) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();
    if (err == 0 && count < MAX_FOUND) {
      found[count++] = addr;
    }
  }
  return count;
}

// print count and list
void printScanResult(const uint8_t found[], int count) {
  if (count == 0) {
    Serial.println("❌ No I²C devices found.");
  } else {
    Serial.printf("✅ Found %d device(s): ", count);
    for (int i = 0; i < count; i++) {
      Serial.printf("0x%02X ", found[i]);
    }
    Serial.println();
  }
}
