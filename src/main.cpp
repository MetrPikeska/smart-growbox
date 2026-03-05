#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

// I2C pins
#define SDA 21
#define SCL 22

// LDR pin (analogový)
#define LDR_PIN 34

// Objekty senzorů
TwoWire I2C_1 = TwoWire(0);
Adafruit_BMP280 bmp280(&I2C_1);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== SmartGrowBox Sensor Test ===\n");
  
  // I2C bus
  I2C_1.begin(SDA, SCL, 100000);
  
  // LDR pin
  pinMode(LDR_PIN, INPUT);
  
  // Inicializace BMP280
  Serial.println("Initializing BMP280...");
  if (!bmp280.begin(0x77)) {
    Serial.println("ERROR: BMP280 not found on address 0x77!");
    if (!bmp280.begin(0x76)) {
      Serial.println("ERROR: BMP280 not found on 0x76 either!");
    }
  } else {
    Serial.println("OK: BMP280 connected");
  }
  
  Serial.println("\n=== Starting measurements ===\n");
}

void loop() {
  // BMP280
  Serial.printf("BMP280: Temp=%.2f°C, Pressure=%.2f hPa, Altitude=%.2f m\n",
    bmp280.readTemperature(),
    bmp280.readPressure() / 100.0,
    bmp280.readAltitude(1013.25)
  );
  
  // LDR
  int ldrValue = analogRead(LDR_PIN);
  float ldrPercent = (ldrValue / 4095.0) * 100.0;
  Serial.printf("LDR: Raw=%d, Light=%.1f%%\n", ldrValue, ldrPercent);
  
  Serial.println("---");
  
  delay(2000);  // Měření každé 2 sekundy
}