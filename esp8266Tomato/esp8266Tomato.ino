#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

const char* ssid = "vivo 1820";      // Nama SSID (jaringan WiFi)
const char* password = "asdfghjkl";  // Kata sandi WiFi

WiFiClientSecure client;

const unsigned long interval = 2000;  // Interval waktu pembacaan data (dalam milidetik)
unsigned long lastMillis = 0;      
float mois1, mois2, mois3;
float hum1, hum2, hum3;
float temp1, temp2, temp3;

void setup() {
  // Serial.begin(115200);
  Serial.begin(9600);
 
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to WiFi. IP Address: ");
  Serial.println(WiFi.localIP());

  client.setInsecure();
}

void loop() {
  // unsigned long currentMillis = millis();
  // if (currentMillis - lastMillis >= interval) {
  //   lastMillis = currentMillis;
  //   sensors_read();

  //   // Kirim data ke server jika terhubung ke WiFi
  //   if (WiFi.status() == WL_CONNECTED) {
  //     sendDataToServer();
  //   }
  // }

  sensors_read();
  if (!(temp1==0 && hum1==0 && mois1==0 && temp2==0 && hum2==0 && mois2==0 && temp3==0 && hum3==0 && mois3==0)){
    // Kirim data ke server jika terhubung ke WiFi
    if (WiFi.status() == WL_CONNECTED) {
        sendDataToServer();
    }
  }
}

void sensors_read() {
  if (Serial.available() > 0) {
    String jsonStr = Serial.readStringUntil('\n');
    DynamicJsonDocument doc(900);

    DeserializationError error = deserializeJson(doc, jsonStr);

    if (!error) {
      temp1 = doc["suhu1"]; 
      hum1 = doc["kelembapan1"]; 
      mois1 = doc["kelembapanTanah1"];
      temp2 = doc["suhu2"]; 
      hum2 = doc["kelembapan2"]; 
      mois2 = doc["kelembapanTanah2"];
      temp3 = doc["suhu3"]; 
      hum3 = doc["kelembapan3"]; 
      mois3 = doc["kelembapanTanah3"];
    } else {
      Serial.print("Gagal parsing JSON: ");
      Serial.println(error.c_str());
    }
  }
}


void sendDataToServer() {
  HTTPClient http;
  http.setTimeout(15000);
  // http.begin(client, "https://8z585jxl-5000.asse.devtunnels.ms/api/sensor");
  http.begin(client, "https://be-ta.vercel.app/api/sensor");
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<900> doc;

  JsonObject plant1 = doc.createNestedObject("plant1");
  plant1["suhu"] = temp1;
  plant1["kelembapan"] = hum1;
  plant1["kelembapanTanah"] = mois1;

  JsonObject plant2 = doc.createNestedObject("plant2");
  plant2["suhu"] = temp2;
  plant2["kelembapan"] = hum2;
  plant2["kelembapanTanah"] = mois2;

  JsonObject plant3 = doc.createNestedObject("plant3");
  plant3["suhu"] = temp3;
  plant3["kelembapan"] = hum3;
  plant3["kelembapanTanah"] = mois3;



  String requestBody;
  serializeJson(doc, requestBody);

  unsigned long startTime = millis();  // Waktu sebelum pengiriman data

  int httpCode = http.POST(requestBody);

  unsigned long endTime = millis();    // Waktu setelah pengiriman data
  unsigned long duration = endTime - startTime;

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("HTTP Code: " + String(httpCode));
    Serial.println("Server Response: " + payload);
    Serial.println("Duration: " + String(duration) + " ms");
  } else {
    Serial.println("Error mengirim request");
    Serial.println(http.errorToString(httpCode));
  }

  http.end();
}

