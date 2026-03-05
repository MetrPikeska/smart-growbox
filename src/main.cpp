#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

// I2C pins
#define SDA 21
#define SCL 22

// Senzor vlhkosti - AO (analog) a DO (digital)
#define SOIL_HUMIDITY_AO1 34  // Soil #1 - Analog
#define SOIL_HUMIDITY_DO1 12  // Soil #1 - Digital
#define SOIL_HUMIDITY_AO2 35  // Soil #2 - Analog  
#define SOIL_HUMIDITY_DO2 13  // Soil #2 - Digital

#define LDR_PIN 32             // Light sensor

// I2C
TwoWire I2C_1 = TwoWire(0);
Adafruit_BMP280 bmp280(&I2C_1);
bool bmp280_ok = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== SmartGrowBox Test ===");
  Serial.println("2x Soil (AO+DO) + BMP280 + LDR\n");
  
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
  pinMode(SOIL_HUMIDITY_DO1, INPUT);
  pinMode(SOIL_HUMIDITY_AO2, INPUT);
  pinMode(SOIL_HUMIDITY_DO2, INPUT);
  pinMode(LDR_PIN, INPUT);
  
  // BMP280
  Serial.println("Init BMP280...");
  if (!bmp280.begin(0x77)) {
    Serial.println("Not at 0x77, trying 0x76...");
    if (!bmp280.begin(0x76)) {
      Serial.println("ERROR: BMP280 not found!");
      bmp280_ok = false;
    } else {
      Serial.println("BMP280 found at 0x76 - OK");
      bmp280_ok = true;
    }
  } else {
    Serial.println("BMP280 found at 0x77 - OK");
    bmp280_ok = true;
  }
  Serial.println();
}

void loop() {
  // Soil #1 - AO + DO
  int soil1AO = analogRead(SOIL_HUMIDITY_AO1);
  int soil1DO = digitalRead(SOIL_HUMIDITY_DO1);
  Serial.printf("Soil #1: AO=%d (%.0f%%), DO=%s\n", 
    soil1AO, (soil1AO/4095.0)*100, soil1DO ? "DRY" : "WET");
  
  // Soil #2 - AO + DO
  int soil2AO = analogRead(SOIL_HUMIDITY_AO2);
  int soil2DO = digitalRead(SOIL_HUMIDITY_DO2);
  Serial.printf("Soil #2: AO=%d (%.0f%%), DO=%s\n", 
    soil2AO, (soil2AO/4095.0)*100, soil2DO ? "DRY" : "WET");
  
  // BMP280
  if (bmp280_ok) {
    Serial.printf("BMP280: T=%.1f°C, P=%.0f hPa\n",
      bmp280.readTemperature(),
      bmp280.readPressure() / 100.0
    );
  } else {
    Serial.println("BMP280: N/A (not initialized)");
  }
  
  // LDR
  int ldr = analogRead(LDR_PIN);
  Serial.printf("LDR: %d (%.0f%%)\n", ldr, (ldr/4095.0)*100);
  
  Serial.println("---");
  delay(2000);
}