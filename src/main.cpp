#include <Arduino.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

// DHT22 (HW-080) pins
#define DHT1_PIN 17
#define DHT2_PIN 18
#define DHTTYPE DHT22

// I2C pins
#define SDA 21
#define SCL 22

// LDR pin (analogový)
#define LDR_PIN 34

// Objekty senzorů
DHT dht1(DHT1_PIN, DHTTYPE);
DHT dht2(DHT2_PIN, DHTTYPE);

TwoWire I2C_1 = TwoWire(0);
Adafruit_BMP280 bmp280(&I2C_1);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== SmartGrowBox Sensor Test ===");
  Serial.println("2x DHT22 (HW-080) + BMP280 + LDR\n");
  
  // I2C bus
  I2C_1.begin(SDA, SCL, 100000);
  
  // LDR pin
  pinMode(LDR_PIN, INPUT);
  
  // Inicializace DHT22 #1
  Serial.println("Initializing DHT22 #1...");
  dht1.begin();
  delay(500);
  Serial.println("OK: DHT22 #1 ready");
  
  // Inicializace DHT22 #2
  Serial.println("Initializing DHT22 #2...");
  dht2.begin();
  delay(500);
  Serial.println("OK: DHT22 #2 ready");
  
  // Inicializace BMP280
  Serial.println("Initializing BMP280...");
  if (!bmp280.begin(0x77)) {
    Serial.println("BMP280 not found on 0x77, trying 0x76...");
    if (!bmp280.begin(0x76)) {
      Serial.println("ERROR: BMP280 not found!");
    } else {
      Serial.println("OK: BMP280 connected on 0x76");
    }
  } else {
    Serial.println("OK: BMP280 connected on 0x77");
  }
  
  Serial.println("\n=== Starting measurements ===\n");
}

void loop() {
  // DHT22 #1
  float temp1 = dht1.readTemperature();
  float hum1 = dht1.readHumidity();
  
  if (isnan(temp1) || isnan(hum1)) {
    Serial.println("DHT22 #1: ERROR reading!");
  } else {
    Serial.printf("DHT22 #1: Temp=%.2f°C, Humidity=%.2f%%\n", temp1, hum1);
  }
  
  // DHT22 #2
  float temp2 = dht2.readTemperature();
  float hum2 = dht2.readHumidity();
  
  if (isnan(temp2) || isnan(hum2)) {
    Serial.println("DHT22 #2: ERROR reading!");
  } else {
    Serial.printf("DHT22 #2: Temp=%.2f°C, Humidity=%.2f%%\n", temp2, hum2);
  }
  
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