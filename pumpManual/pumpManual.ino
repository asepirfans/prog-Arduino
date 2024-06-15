#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const unsigned long interval = 1000;  // Polling setiap 10 detik
unsigned long lastMillis = 0;   

const char* ssid = "vivo 1820";      // Nama SSID (jaringan WiFi)
const char* password = "asdfghjkl";  // Kata sandi WiFi
const char *apiUrl = "https://d4ts3cxh-5000.asse.devtunnels.ms/api/pump"; 

const int relay = D2;

void setup()
{
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
  

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);

  // Tunggu koneksi WiFi berhasil
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Connected to WiFi. IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis >= interval) {
    lastMillis = currentMillis;
    getData();
  }
}

void getData() {
  // Ambil data dari API
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.begin(client, apiUrl);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.GET();
  if (httpCode > 0)
  {
    String payload = http.getString();
    Serial.println(payload);

    // Parse JSON
    StaticJsonDocument<500> doc;
    deserializeJson(doc, payload);

    // Dapatkan nilai dari field "pump1"
    const char *pump1Status = doc["data"]["pump1"];
    std::string pump1StatusStr = pump1Status;

    static std::string previousPump1Status = ""; // Variabel untuk menyimpan 
      if (pump1StatusStr == "ON")
      {
        digitalWrite(relay, 0); // Hidupkan relay
        Serial.println("Pump is ON");
      }
      else if (pump1StatusStr == "OFF")
      {
        digitalWrite(relay, 1); // Matikan relay
        Serial.println("Pump is OFF");
      }
  }
  else
  {
    Serial.printf("HTTP request failed with error %d\n", httpCode);
  }

  http.end();
  delay(1000);
}
