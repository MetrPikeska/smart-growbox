#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_AHTX0.h>

// I2C pins
#define SDA 21
#define SCL 22

// Senzor vlhkosti - jen analog
#define SOIL_HUMIDITY_AO1 34  // Soil #1 - Analog
#define SOIL_HUMIDITY_AO2 35  // Soil #2 - Analog

#define LDR_PIN 32             // Light sensor

// I2C
TwoWire I2C_1 = TwoWire(0);
Adafruit_AHTX0 aht10;
bool aht10_ok = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== SmartGrowBox Test ===");
  Serial.println("2x Soil (Analog) + AHT10 + LDR\n");
  
  // I2C
  I2C_1.begin(SDA, SCL, 100000);
  
  // Skenuj I2C senzory
  Serial.println("Scanning I2C devices...");
  for (int i = 8; i < 120; i++) {
    I2C_1.beginTransmission(i);
    if (I2C_1.endTransmission() == 0) {
      Serial.printf("I2C device found at 0x%X\n", i);
    }
  }
  Serial.println("Scan complete!\n");
  
  // Piny
  pinMode(SOIL_HUMIDITY_AO1, INPUT);
  pinMode(SOIL_HUMIDITY_AO2, INPUT);
  pinMode(LDR_PIN, INPUT);
  
  // BMP280
  Serial.println("Init AHT10...");
  if (!aht10.begin(&I2C_1, 0x38)) {
    Serial.println("ERROR: AHT10 not found!");
    aht10_ok = false;
  } else {
    Serial.println("AHT10 found - OK");
    aht10_ok = true;
  }
  Serial.println();
}

void loop() {
  // Soil #1 - Analog
  int soil1AO = analogRead(SOIL_HUMIDITY_AO1);
  Serial.printf("Soil #1: %d (%.1f%%)\n", 
    soil1AO, (soil1AO/4095.0)*100);
  
  // Soil #2 - Analog
  int soil2AO = analogRead(SOIL_HUMIDITY_AO2);
  Serial.printf("Soil #2: %d (%.1f%%)\n", 
    soil2AO, (soil2AO/4095.0)*100);
  
  // AHT10
  if (aht10_ok) {
    sensors_event_t humidity, temp;
    aht10.getEvent(&humidity, &temp);
    Serial.printf("AHT10: T=%.1f°C, H=%.1f%%\n",
      temp.temperature,
      humidity.relative_humidity
    );
  } else {
    Serial.println("AHT10: N/A (not initialized)");
  }
  
  // LDR
  int ldr = analogRead(LDR_PIN);
  Serial.printf("LDR: %d (%.0f%%)\n", ldr, (ldr/4095.0)*100);
  
  Serial.println("---");
  delay(2000);
}