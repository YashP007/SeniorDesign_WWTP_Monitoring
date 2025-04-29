#include <Wire.h>

// I²C on custom pins  
static const uint8_t SDA_PIN   = 6;  
static const uint8_t SCL_PIN   = 7;  

// MENB pins for 4 x LMP91000  
static const uint8_t menbPins[4] = { 2, 3, 4, 5 };  

// on-board LED  
static const uint8_t LED_PIN    = 9;  

// LMP91000 I²C address (7-bit)  
static const uint8_t LMP_ADDR   = 0x48;  

// Registers  
static const uint8_t REG_LOCK   = 0x01;  
static const uint8_t REG_TIACN  = 0x10;  
static const uint8_t REG_REFCN  = 0x11;  
static const uint8_t REG_MODECN = 0x12;  

// Common configuration values  
// TIACN: external Rtia, Rload=10Ω  
static const uint8_t TIACN_CFG = (0b000 << 5) | (0b00 << 0);  
// REFCN: internal ref, 50% divider, zero bias  
static const uint8_t REFCN_CFG = (0 << 7) | (0b01 << 5) | (0 << 4) | (0b0000 << 0);  
// MODECN: 3-lead amperometric  
static const uint8_t MODECN_CFG = (0 << 7) | (0b011 << 0);  

// helper to write one byte into a register  
void write8(uint8_t reg, uint8_t val) {  
  Wire.beginTransmission(LMP_ADDR);  
  Wire.write(reg);  
  Wire.write(val);  
  Wire.endTransmission();  
}  

// disable all sensors by pulling MENB high  
void disableAll() {  
  for (int i = 0; i < 4; i++) digitalWrite(menbPins[i], HIGH);  
}  

// enable exactly one sensor, leave others disabled  
void enableOne(int idx) {  
  disableAll();  
  digitalWrite(menbPins[idx], LOW);  
  delayMicroseconds(10);    // allow MENB settle  
}  

// configure a single sensor (assumes it's already enabled)  
void configOne() {  
  // unlock TIACN & REFCN  
  write8(REG_LOCK, 0x00);  
  // set TIA & load  
  write8(REG_TIACN, TIACN_CFG);  
  // set internal reference divider & bias  
  write8(REG_REFCN, REFCN_CFG);  
  // set operating mode  
  write8(REG_MODECN, MODECN_CFG);  
}  

void setup() {  
  Serial.begin(115200);  
  while(!Serial);  

  // LED  
  pinMode(LED_PIN, OUTPUT);  
  digitalWrite(LED_PIN, LOW);  

  // MENB pins  
  for (int i = 0; i < 4; i++) {  
    pinMode(menbPins[i], OUTPUT);  
    digitalWrite(menbPins[i], HIGH);  // start disabled  
  }  

  // init I²C  
  Wire.begin(SDA_PIN, SCL_PIN);  
  Wire.setClock(100000);  

  Serial.println(F("LMP91000 x4 TIA scan"));  

  // configure all devices in turn  
  for (int i = 0; i < 4; i++) {  
    enableOne(i);  
    configOne();  
    digitalWrite(LED_PIN, HIGH);  
    delay(20);  
    digitalWrite(LED_PIN, LOW);  
  }  
  disableAll();  
  Serial.println(F("All sensors configured."));  
}  

void loop() {  
  // read each sensor’s output  
  for (int i = 0; i < 4; i++) {  
    enableOne(i);  
    // small pause to let TIA output settle  
    delay(5);  

    int raw = analogRead(A0);               // 0–1023 at 5V ref  
    float volts = raw * (5.0 / 1023.0);      // convert to V  
    Serial.print("Sensor ");  
    Serial.print(i);  
    Serial.print(": ");  
    Serial.print(volts, 3);  
    Serial.print(" V");  
    if (i < 3) Serial.print(" \t");  
  }  
  disableAll();  

  // blink LED to show end of cycle  
  digitalWrite(LED_PIN, HIGH);  
  delay(50);  
  digitalWrite(LED_PIN, LOW);  

  Serial.println();  
  delay(500);  
}  
