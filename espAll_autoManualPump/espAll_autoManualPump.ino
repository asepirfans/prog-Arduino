//------------------Library---------------------
#include <WiFi.h>  
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
WiFiClientSecure client;

//------------------WIFI---------------------
const char* ssid = "vivo 1820";      // Nama SSID (jaringan WiFi)
const char* password = "asdfghjkl";  // Kata sandi WiFi

//------------------DHT 11---------------------
#define DHTPIN1 32 
#define DHTPIN2 33
#define DHTPIN3 25

#define DHTTYPE DHT11

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
DHT dht3(DHTPIN3, DHTTYPE);

//------------------Soil Moisture---------------------
#define SOILPIN1 36
#define SOILPIN2 39
#define SOILPIN3 34

const unsigned long interval = 2000;
unsigned long lastMillis = 0;

float mois1, mois2, mois3;
float hum1, hum2, hum3;
float temp1, temp2, temp3;
float soilMoisture1, soilMoisture2, soilMoisture3;
float temperature1, temperature2, temperature3;
float humidity1, humidity2, humidity3;

//------------------Relay---------------------
const int relay1 = 19;
const int relay2 = 18;
const int relay3 = 5;

int bitADC = 4095;

void setup() {
  Serial.begin(115200);
  dht1.begin();
  dht2.begin();
  dht3.begin();
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);

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
  //   suhuRead();
  //   soilMoistureRead();
  //   pump();

  //   // Kirim data ke server jika terhubung ke WiFi
  //   if (WiFi.status() == WL_CONNECTED) {
  //     sendDataToServer();
  //   }

    
  // }
  suhuRead();
  soilMoistureRead();
  autoManual();
  // getData();
  // pump();
  if (!(temp1==0 && hum1==0 && mois1==0 && temp2==0 && hum2==0 && mois2==0 && temp3==0 && hum3==0 && mois3==0)){
    // Kirim data ke server jika terhubung ke WiFi
    if (WiFi.status() == WL_CONNECTED) {
        sendDataToServer();
    }
  }
}

void suhuRead(){
  humidity1 = dht1.readHumidity();
  temperature1 = dht1.readTemperature();
  humidity2 = dht2.readHumidity();
  temperature2 = dht2.readTemperature();
  humidity3 = dht3.readHumidity();
  temperature3 = dht3.readTemperature();

  // Konversi ke format desimal dengan satu angka di belakang koma
  hum1 = round(humidity1 * 10.0) / 10.0;
  temp1 = round(temperature1 * 10.0) / 10.0;
  hum2 = round(humidity2 * 10.0) / 10.0;
  temp2 = round(temperature2 * 10.0) / 10.0;
  hum3 = round(humidity3 * 10.0) / 10.0;
  temp3 = round(temperature3 * 10.0) / 10.0;

  // Serial.println("Sensor DHT1:");
  // Serial.print("Humidity: ");
  // Serial.print(hum1);
  // Serial.print("%  Temperature: ");
  // Serial.println(temp1);  

  // Serial.println("Sensor DHT2:");
  // Serial.print("Humidity: ");
  // Serial.print(humidity2);
  // Serial.print("%  Temperature: ");
  // Serial.println(temperature2); 

  // Serial.println("Sensor DHT3:");
  // Serial.print("Humidity: ");
  // Serial.print(humidity3);
  // Serial.print("%  Temperature: ");
  // Serial.println(temperature3);   
}

void soilMoistureRead(){
  float adcValue1 = analogRead(SOILPIN1);
  soilMoisture1 = (100 - ((adcValue1 / bitADC) * 100));

  float adcValue2 = analogRead(SOILPIN2);
  soilMoisture2 = (100 - ((adcValue2 / bitADC) * 100));

  float adcValue3 = analogRead(SOILPIN3);
  soilMoisture3 = (100 - ((adcValue3 / bitADC) * 100));

  mois1 = round(soilMoisture1 * 10.0) / 10.0;
  mois2 = round(soilMoisture2 * 10.0) / 10.0;
  mois3 = round(soilMoisture3 * 10.0) / 10.0;

  // Serial.println("Sensor Soil1:");
  // Serial.print("Moisture1: ");
  // Serial.print(mois1);
  // Serial.print("%  adc: ");
  // Serial.print(adcValue1);
  // Serial.println("");

  // Serial.println("Sensor Soil2:");
  // Serial.print("Moisture2: ");
  // Serial.print(soilMoisture2);
  //  Serial.println("");

  // Serial.println("Sensor Soil3:");
  // Serial.print("Moisture: ");
  // Serial.print(soilMoisture3);

}

void pump() {
  if (mois1 > 60 ) {
    digitalWrite(relay1, LOW);
  } else {
    digitalWrite(relay1, HIGH);
  }
  if (mois2 > 60 ) {
    digitalWrite(relay2, LOW);
  } else {
    digitalWrite(relay2, HIGH);
  }
  if (mois3 > 60 ) {
    digitalWrite(relay3, LOW);
  } else {
    digitalWrite(relay3, HIGH);
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

void getData() {
  // Ambil data dari API
  const char *apiUrl = "https://be-ta.vercel.app/api/pump"; 
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
    const char *pump2Status = doc["data"]["pump2"];
    const char *pump3Status = doc["data"]["pump3"];
    std::string pump1StatusStr = pump1Status;
    std::string pump2StatusStr = pump2Status;
    std::string pump3StatusStr = pump3Status;

    static std::string previousPump1Status = "";
    static std::string previousPump2Status = "";
    static std::string previousPump3Status = "";
      if (pump1StatusStr == "ON")
      {
        digitalWrite(relay1, 1); // Hidupkan relay
        Serial.println("Pump is ON");
      }
      else if (pump1StatusStr == "OFF")
      {
        digitalWrite(relay1, 0); // Matikan relay
        Serial.println("Pump is OFF");
      }
      if (pump2StatusStr == "ON")
      {
        digitalWrite(relay2, 1); // Hidupkan relay
        Serial.println("Pump is ON");
      }
      else if (pump2StatusStr == "OFF")
      {
        digitalWrite(relay2, 0); // Matikan relay
        Serial.println("Pump is OFF");
      }
      if (pump3StatusStr == "ON")
      {
        digitalWrite(relay3, 1); // Hidupkan relay
        Serial.println("Pump is ON");
      }
      else if (pump3StatusStr == "OFF")
      {
        digitalWrite(relay3, 0); // Matikan relay
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


void autoManual() {
  // Ambil data dari API
  const char *apiUrl = "https://d4ts3cxh-5000.asse.devtunnels.ms/api/pump";
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
    const char *autoManual = doc["data"]["autoManual"];
    std::string autoManualStr = autoManual;

    static std::string previousAutoManual = ""; // Variabel untuk menyimpan 
      if (autoManualStr == "Manual")
      {
       getData();
      }
      else if (autoManualStr == "Auto")
      {
       pump();
      }
  }
  else
  {
    Serial.printf("HTTP request failed with error %d\n", httpCode);
  }

  http.end();
  delay(1000);
}

