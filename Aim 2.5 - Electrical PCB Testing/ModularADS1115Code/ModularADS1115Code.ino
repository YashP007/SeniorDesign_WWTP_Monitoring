#include <Wire.h>

// custom I²C pins
static const uint8_t SDA_PIN = 6, SCL_PIN = 7;

// ADS1015 address
#define ADS_ADDR    0x48

// register pointers
#define REG_CONV    0x00
#define REG_CONFIG  0x01

// CONFIG bit‐fields for ADS1015 (same as ADS1115)
#define OS_SINGLE        0x8000  // start single-shot
#define MUX_AIN0         0x4000  // AIN0 vs GND
#define PGA_2_048V       0x0400  // ±2.048 V
#define MODE_SINGLE      0x0100  // single-shot
#define DR_128SPS        0x0080  // 128 SPS
#define COMP_QUE_DISABLE 0x0003  // disable comparator

uint16_t cfg;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  // build config word
  cfg = OS_SINGLE | MUX_AIN0 | PGA_2_048V | MODE_SINGLE | DR_128SPS | COMP_QUE_DISABLE;
}

void loop() {
  // trigger conversion
  writeReg(REG_CONFIG, cfg);
  delayMicroseconds(1);  // wait for conversion (~1/128s)

  // read raw 16-bit
  uint16_t raw16 = readReg(REG_CONV);
  // drop low nibble → real 12-bit result
  //int16_t counts12 = raw16 >> 4;

  // for 12-bit two’s-complement full-scale = ±2048 counts
  //const float lsb_uV = 2.048e6f / 2048.0f;  // =1000 µV/count
  //float mV = counts12 * lsb_uV / 1000.0f;

  //Serial.print("raw16=0x"); 
  Serial.println(raw16, HEX);
  //Serial.print("  counts12="); Serial.print(counts12);
  //Serial.print("  V= "); Serial.print(mV, 3); Serial.println(" mV");

  delayMicroseconds(1);
}

// write 16 bits
void writeReg(uint8_t reg, uint16_t val) {
  Wire.beginTransmission(ADS_ADDR);
  Wire.write(reg);
  Wire.write(val >> 8);
  Wire.write(val & 0xFF);
  Wire.endTransmission();
}

// read 16 bits
uint16_t readReg(uint8_t reg) {
  Wire.beginTransmission(ADS_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(ADS_ADDR, (uint8_t)2);
  uint16_t hi = Wire.read();
  uint16_t lo = Wire.read();
  return (hi << 8) | lo;
}
