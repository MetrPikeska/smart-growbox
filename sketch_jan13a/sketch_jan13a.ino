#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

/* ===== WIFI ===== */
const char* ssid     = "Tomsovsky";
const char* password = "604246127";

/* ===== I2C ===== */
#define I2C_SDA 18
#define I2C_SCL 19

/* ===== SOIL ===== */
#define SOIL_POWER 25
#define SOIL_PIN   34
#define SOIL_DRY   3200
#define SOIL_WET   1400

/* ===== LDR ===== */
#define LDR_PIN    32
#define LDR_DARK   3800
#define LDR_LIGHT  800

/* ===== TCRT5000 ===== */
#define TCRT_PIN   35
#define TCRT_NEAR  3500
#define TCRT_FAR   500

WebServer server(80);
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

bool aht_ok = false;
bool bmp_ok = false;

/* ===== DATA ===== */
float temp_c = 0;
float hum_p = 0;
float press_hpa = 0;

int soil_raw = 0;
float soil_pct = 0;

int ldr_raw = 0;
float light_pct = 0;

int tcrt_raw = 0;
float proximity_pct = 0;

/* ===== WEB UI ===== */
void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='utf-8'>";
  html += "<meta http-equiv='refresh' content='5'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body{font-family:Arial;background:#0f172a;color:#e5e7eb;padding:20px}";
  html += ".card{background:#111827;padding:20px;border-radius:12px;max-width:380px}";
  html += "h1{font-size:22px}";
  html += ".lbl{color:#9ca3af}";
  html += ".val{font-size:26px;margin-bottom:10px}";
  html += "</style></head><body>";

  html += "<div class='card'><h1>ESP32 Sensor Hub</h1>";

  html += "<div class='lbl'>Temperature</div><div class='val'>" + String(temp_c,1) + " °C</div>";
  html += "<div class='lbl'>Humidity (air)</div><div class='val'>" + String(hum_p,1) + " %</div>";
  html += "<div class='lbl'>Pressure</div><div class='val'>" + String(press_hpa,1) + " hPa</div>";

  html += "<div class='lbl'>Soil moisture</div><div class='val'>" + String(soil_pct,1) + " %</div>";
  html += "<div class='lbl'>Light level</div><div class='val'>" + String(light_pct,1) + " %</div>";
  html += "<div class='lbl'>Proximity (TCRT5000)</div><div class='val'>" + String(proximity_pct,1) + " %</div>";

  html += "<div class='lbl'>Auto refresh 5 s</div>";
  html += "</div></body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(I2C_SDA, I2C_SCL);

  pinMode(SOIL_POWER, OUTPUT);
  digitalWrite(SOIL_POWER, LOW);

  aht_ok = aht.begin();
  bmp_ok = bmp.begin(0x76);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();

  /* AHT20 */
  if (aht_ok) {
    sensors_event_t h, t;
    aht.getEvent(&h, &t);
    temp_c = t.temperature;
    hum_p = h.relative_humidity;
  }

  /* BMP280 */
  if (bmp_ok) {
    press_hpa = bmp.readPressure() / 100.0;
  }

  /* SOIL */
  digitalWrite(SOIL_POWER, HIGH);
  delay(10);
  soil_raw = analogRead(SOIL_PIN);
  digitalWrite(SOIL_POWER, LOW);
  soil_pct = map(soil_raw, SOIL_DRY, SOIL_WET, 0, 100);
  soil_pct = constrain(soil_pct, 0, 100);

  /* LDR */
  ldr_raw = analogRead(LDR_PIN);
  light_pct = map(ldr_raw, LDR_DARK, LDR_LIGHT, 0, 100);
  light_pct = constrain(light_pct, 0, 100);

  /* TCRT5000 */
  tcrt_raw = analogRead(TCRT_PIN);
  proximity_pct = map(tcrt_raw, TCRT_FAR, TCRT_NEAR, 0, 100);
  proximity_pct = constrain(proximity_pct, 0, 100);

  Serial.printf(
    "T:%.1f H:%.1f P:%.1f | Soil:%d(%.0f%%) Light:%d(%.0f%%) TCRT:%d(%.0f%%)\n",
    temp_c, hum_p, press_hpa,
    soil_raw, soil_pct,
    ldr_raw, light_pct,
    tcrt_raw, proximity_pct
  );

  delay(2000);
}
