#include <ArduinoJson.h>
#include <DHT.h>

//------------------Source VIN---------------------

// int pinRef = A0;
// float vRef;
int bitADC = 1023;

//------------------DHT 11---------------------
#define DHTPIN1 2 
#define DHTPIN2 4
#define DHTPIN3 7

#define DHTTYPE DHT11

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
DHT dht3(DHTPIN3, DHTTYPE);

//------------------Soil Moisture---------------------
#define SOILPIN1 A1
#define SOILPIN2 A2
#define SOILPIN3 A3

const unsigned long interval = 2000;
unsigned long lastMillis = 0;

float mois1, mois2, mois3;
float hum1, hum2, hum3;
float temp1, temp2, temp3;
float soilMoisture1, soilMoisture2, soilMoisture3;
float temperature1, temperature2, temperature3;
float humidity1, humidity2, humidity3;

//------------------Relay---------------------
const int relay1 = 8;
const int relay2 = 9;
const int relay3 = 10;


void setup() {
  // Serial.begin(9600);
  Serial.begin(115200);
  dht1.begin();
  dht2.begin();
  dht3.begin();
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  // pinMode(3, OUTPUT);
  // digitalWrite(3, HIGH);

}

// void sourceRead(){
//   vRef = analogRead(pinRef)*5/bitADC;

//   // Serial.print("source: ");
//   // Serial.print(vRef);
//   // Serial.println(" volt");
// }

void suhuRead(){
  hum1 = dht1.readHumidity();
  temp1 = dht1.readTemperature();
  hum2 = dht2.readHumidity();
  temp2 = dht2.readTemperature();
  hum3 = dht3.readHumidity();
  temp3 = dht3.readTemperature();

  // Konversi ke format desimal dengan satu angka di belakang koma
  humidity1 = round(hum1 * 10.0) / 10.0;
  temperature1 = round(temp1 * 10.0) / 10.0;
  humidity2 = round(hum2 * 10.0) / 10.0;
  temperature2 = round(temp2 * 10.0) / 10.0;
  humidity3 = round(hum3 * 10.0) / 10.0;
  temperature3 = round(temp3 * 10.0) / 10.0;

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
  mois1 = (100 - ((adcValue1 / bitADC) * 100));

  //  if (mois1 > 60 ) {
  //   digitalWrite(3, HIGH);
  // } else {
  //   digitalWrite(3, LOW);
  // }

  float adcValue2 = analogRead(SOILPIN2);
  mois2 = (100 - ((adcValue2 / bitADC) * 100));

  float adcValue3 = analogRead(SOILPIN3);
  mois3 = (100 - ((adcValue3 / bitADC) * 100));

  soilMoisture1 = round(mois1 * 10.0) / 10.0;
  soilMoisture2 = round(mois2 * 10.0) / 10.0;
  soilMoisture3 = round(mois3 * 10.0) / 10.0;

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


//------------------OUTPUT---------------------
void pump() {
  if (soilMoisture1 > 60 ) {
    digitalWrite(relay1, LOW);
  } else {
    digitalWrite(relay1, HIGH);
  }
  if (soilMoisture2 > 60 ) {
    digitalWrite(relay2, LOW);
  } else {
    digitalWrite(relay2, HIGH);
  }
  if (soilMoisture3 > 60 ) {
    digitalWrite(relay3, LOW);
  } else {
    digitalWrite(relay3, HIGH);
  }

}

//------------------Komunikasi Serial---------------------
void sensorWrite() {
  // Buat objek JSON
  StaticJsonDocument<900> doc;
  doc["suhu1"] = temp1;
  doc["kelembapan1"] = hum1; 
  doc["kelembapanTanah1"] = soilMoisture1; 

  doc["suhu2"] = temp2;
  doc["kelembapan2"] = hum2; 
  doc["kelembapanTanah2"] = soilMoisture2; 
  
  doc["suhu3"] = temp3;
  doc["kelembapan3"] = hum3; 
  doc["kelembapanTanah3"] = soilMoisture3; 

  // Serialisasi objek JSON ke string
  String jsonString;
  serializeJson(doc, jsonString);

  // Kirim JSON ke ESP32
  Serial.println(jsonString);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastMillis >= interval) {
  lastMillis = currentMillis;
    pump();
    suhuRead();
    soilMoistureRead();

    sensorWrite();
  }
}
