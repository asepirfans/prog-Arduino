//------------------Library---------------------
#include <WiFi.h>  
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
// #include <LiquidCrystal_I2C.h>
WiFiClientSecure client;

//------------------WIFI---------------------
const char* ssid = "Kosan cerdas";      
const char* password = "K054NC3RD45"; 
 
// const char* ssid = "vivo 1820";      
// const char* password = "asdfghjkl";  

//------------------DHT 11---------------------
#define DHTPIN1 32 
#define DHTPIN2 33
#define DHTPIN3 25
// #define DHTPIN1 17 
// #define DHTPIN2 33
// #define DHTPIN3 16


#define DHTTYPE DHT11
// #define DHTTYPE2 DHT22

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

//------------------lCD I2C------------------------
// LiquidCrystal_I2C lcd(0x27, 16, 2);

int bitADC = 4095;

void setup() {
  // lcd.begin();
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

// void loop() {
//   // unsigned long currentMillis = millis();
//   // if (currentMillis - lastMillis >= interval) {
//   //   lastMillis = currentMillis;
//   //   suhuRead();
//   //   soilMoistureRead();
//   //   pump();

//   //   // Kirim data ke server jika terhubung ke WiFi
//   //   if (WiFi.status() == WL_CONNECTED) {
//   //     sendDataToServer();
//   //   }

    
//   // }

//   if (WiFi.status() == WL_CONNECTED) {
//     suhuRead();
//     pump();
//     soilMoistureRead();
//     sendDataToServer();
//   }
  
//   // if (!(temp1==0 && hum1==0 && mois1==0 && temp2==0 && hum2==0 && mois2==0 && temp3==0 && hum3==0 && mois3==0)){
//   //   if (WiFi.status() == WL_CONNECTED) {
//   //       sendDataToServer();
//   //   }
//   // }
// }

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    suhuRead();
    soilMoistureRead();
    pump();
    sendDataToServer();
    delay(500);  
  }
}


void pump() {
  if (mois1 > 100 ) {
    digitalWrite(relay1, LOW);
  } else {
    digitalWrite(relay1, HIGH);
  }
  if (mois2 > 100 ) {
    digitalWrite(relay2, LOW);
  } else {
    digitalWrite(relay2, HIGH);
  }
  if (mois3 > 100) {
    digitalWrite(relay3, LOW);
  } else {
    digitalWrite(relay3, HIGH);
  }
}

void suhuRead() {
  humidity1 = dht1.readHumidity();
  temperature1 = dht1.readTemperature();
  humidity2 = dht2.readHumidity();
  temperature2 = dht2.readTemperature();
  humidity3 = dht3.readHumidity();
  temperature3 = dht3.readTemperature();

  // Cek dan ganti NaN dengan 0
  if (isnan(humidity1)) {
    humidity1 = 0;
  }
  if (isnan(temperature1)) {
    temperature1 = 0;
  }
  if (isnan(humidity2)) {
    humidity2 = 0;
  }
  if (isnan(temperature2)) {
    temperature2 = 0;
  }
  if (isnan(humidity3)) {
    humidity3 = 0;
  }
  if (isnan(temperature3)) {
    temperature3 = 0;
  }

  // Konversi ke format desimal dengan satu angka di belakang koma
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
  Serial.print(hum2);
  Serial.print("%  Temperature: ");
  Serial.println(temp2);

  Serial.println("Sensor DHT3:");
  Serial.print("Humidity: ");
  Serial.print(hum3);
  Serial.print("%  Temperature: ");
  Serial.println(temp3);
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

  Serial.println("Sensor Soil1:");
  Serial.print("Moisture1: ");
  Serial.print(mois1);
  Serial.print("%  adc: ");
  Serial.print(adcValue1);
  Serial.println("");

  Serial.println("Sensor Soil2:");
  Serial.print("Moisture2: ");
  Serial.print(soilMoisture2);
   Serial.println("");

  Serial.println("Sensor Soil3:");
  Serial.print("Moisture: ");
  Serial.print(soilMoisture3);

}

// void pump() {
//   if (mois1 > 60 ) {
//     digitalWrite(relay1, LOW);
//   } else {
//     digitalWrite(relay1, HIGH);
//   }
//   if (mois2 > 60 ) {
//     digitalWrite(relay2, LOW);
//   } else {
//     digitalWrite(relay2, HIGH);
//   }
//   if (mois3 > 60 ) {
//     digitalWrite(relay3, LOW);
//   } else {
//     digitalWrite(relay3, HIGH);
//   }

// }

// void lcd_show(){

//   lcd.clear();
//   lcd.setCursor(0,0);
//   lcd.print("Suhu1:");
//   lcd.print(temp1);
//   lcd.write(223);
//   lcd.print("C");
//   lcd.setCursor(0,1);
//   lcd.print("Mois1:");
//   lcd.print(mois1);
//   delay(2000);

//   lcd.clear();
//   lcd.setCursor(0,0);
//   lcd.print("Suhu2:");
//   lcd.print(temp2);
//   lcd.write(223);
//   lcd.print("C");
//   lcd.setCursor(0,1);
//   lcd.print("Mois2:");
//   lcd.print(mois2);
//   delay(2000);

//   lcd.clear();
//   lcd.setCursor(0,0);
//   lcd.print("Suhu3:");
//   lcd.print(temp3);
//   lcd.write(223);
//   lcd.print("C");
//   lcd.setCursor(0,1);
//   lcd.print("Mois3:");
//   lcd.print(mois3);
//   delay(2000);
// }


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

