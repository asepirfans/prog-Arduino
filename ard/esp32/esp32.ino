#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
WiFiClientSecure client;

const char* ssid = "Kosan cerdas";
const char* password = "K054NC3RD45";

#define DHTPIN1 32 
#define DHTPIN2 33
#define DHTPIN3 25
#define DHTTYPE DHT11

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
DHT dht3(DHTPIN3, DHTTYPE);

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

LiquidCrystal_I2C lcd(0x27, 16, 2);
int bitADC = 4095;

void setup() {
  Serial.begin(115200);
  dht1.begin();
  dht2.begin();
  dht3.begin();

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
  soilMoistureRead();
  suhuRead();

  if (WiFi.status() == WL_CONNECTED) {
    sendDataToServer();
  }
}

void suhuRead() {
  humidity1 = dht1.readHumidity();
  temperature1 = dht1.readTemperature();
  humidity2 = dht2.readHumidity();
  temperature2 = dht2.readTemperature();
  humidity3 = dht3.readHumidity();
  temperature3 = dht3.readTemperature();

  hum1 = round(humidity1 * 10.0) / 10.0;
  temp1 = round(temperature1 * 10.0) / 10.0;
  hum2 = round(humidity2 * 10.0) / 10.0;
  temp2 = round(temperature2 * 10.0) / 10.0;
  hum3 = round(humidity3 * 10.0) / 10.0;
  temp3 = round(temperature3 * 10.0) / 10.0;

  Serial.println("Sensor DHT1:");
  Serial.print("Humidity: ");
  Serial.print(hum1);
  Serial.print("%  Temperature: ");
  Serial.println(temp1);  

  Serial.println("Sensor DHT2:");
  Serial.print("Humidity: ");
  Serial.print(humidity2);
  Serial.print("%  Temperature: ");
  Serial.println(temperature2); 

  Serial.println("Sensor DHT3:");
  Serial.print("Humidity: ");
  Serial.print(humidity3);
  Serial.print("%  Temperature: ");
  Serial.println(temperature3);  
}

void soilMoistureRead() {
  float adcValue1 = analogRead(SOILPIN1);
  soilMoisture1 = (100 - ((adcValue1 / bitADC) * 100));

  float adcValue2 = analogRead(SOILPIN2);
  soilMoisture2 = (100 - ((adcValue2 / bitADC) * 100));

  float adcValue3 = analogRead(SOILPIN3);
  soilMoisture3 = (100 - ((adcValue3 / bitADC) * 100));

  mois1 = round(soilMoisture1 * 10.0) / 10.0;
  mois2 = round(soilMoisture2 * 10.0) / 10.0;
  mois3 = round(soilMoisture3 * 10.0) / 10.0;

  Serial.print("SOIL1:");
  Serial.println(mois1);
  Serial.print("SOIL2:");
  Serial.println(mois2);
  Serial.print("SOIL3:");
  Serial.println(mois3);
}

void sendDataToServer() {
  HTTPClient http;
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

  unsigned long startTime = millis(); 
  int httpCode = http.POST(requestBody);
  unsigned long endTime = millis();    
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
