#include <WiFi.h>  
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// const char* ssid = "vivo 1820";     
// const char* password = "asdfghjkl"; 

const char* ssid = "Kosan cerdas";
const char* password = "K054NC3RD45";

WiFiClientSecure client;

const unsigned long interval = 2000;  // Interval waktu pembacaan data (dalam milidetik)
unsigned long lastMillis = 0;      
float mois1, mois2, mois3;
float hum1, hum2, hum3;
float temp1, temp2, temp3;

// #define relay1 25
// #define relay2 26
// #define relay3 27

void setup() {
  Serial.begin(115200);
  // pinMode(relay1, OUTPUT);
  // pinMode(relay2, OUTPUT);
  // pinMode(relay3, OUTPUT);

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
  // pump();
  if (!(temp1==0 && hum1==0 && mois1==0 && temp2==0 && hum2==0 && mois2==0 && temp3==0 && hum3==0 && mois3==0)){
    // Kirim data ke server jika terhubung ke WiFi
    if (WiFi.status() == WL_CONNECTED) {
        sendDataToServer();
    }
  }
}



void sensors_read(){
  if (Serial.available() > 0) {
    // Baca data JSON dari Arduino
    String jsonStr = Serial.readStringUntil('\n');

    // Parse JSON
    DynamicJsonDocument doc(900);
    DeserializationError error = deserializeJson(doc, jsonStr);

    // Cek kesalahan parsing
    if (error) {
      Serial.print("Gagal parsing JSON: ");
      Serial.println(error.c_str());
      return;
    }

    // Ambil nilai sensor dari JSON
    temp1 = doc["suhu1"];
    hum1 = doc["kelembapan1"];
    mois1 = doc["kelembapanTanah1"];

    temp2 = doc["suhu2"];
    hum2 = doc["kelembapan2"];
    mois2 = doc["kelembapanTanah2"];

    temp3 = doc["suhu3"];
    hum3 = doc["kelembapan3"];
    mois3 = doc["kelembapanTanah3"];
  }
}

void sendDataToServer() {
  HTTPClient http;
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

// void pump(){
//   if (mois1 > 40) {
//     digitalWrite(relay1, LOW);  
//   } else {
//     digitalWrite(relay1, HIGH);  
//   }
//   if (mois2 > 40) {
//     digitalWrite(relay2, LOW);  
//   } else {
//     digitalWrite(relay2, HIGH);  
//   }
//   if (mois3 > 40) {
//     digitalWrite(relay3, LOW);  
//   } else {
//     digitalWrite(relay3, HIGH);  
//   }
// }

