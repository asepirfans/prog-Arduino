#include <ESP8266Firebase.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <RTClib.h>

RTC_DS4241 rtc;
String timestamp;

#define _SSID "Probitas"          // Nama SSID WiFi Anda                                                                                                                                                                                                                                                                                                                                                                                4
#define _PASSWORD "bugibugi"   // Kata Sandi WiFi Anda
#define REFERENCE_URL "https://flutter-v1-42b4d-default-rtdb.asia-southeast1.firebasedatabase.app/"  // URL referensi proyek Firebase Anda
#define FIREBASE_AUTH "hJ4r7hjzVw5AqIKjllkTJblXhxkFjr4qjlAM5X75"  // Secret Firebase Anda
Firebase firebase(REFERENCE_URL);

const int proximity1 = D0;
const int proximity2 = D1;
int lastSensorValue1 = -1;
int lastSensorValue2 = -1;
int detectionCounter = firebase.getInt("detectionCounter");
unsigned long startTime = 0;  // Declare startTime outside the loop
int acknowledge = 0;
unsigned long lastGraphId = 0;
unsigned long lastStatusId = 0;

//program sensor kecepatan
# define sensorPin D2
const float pi = 3.14159265; // pi number
int period = 2000;           // Measurement period (milliseconds)
int radio = 22;              // Distance from center wheel to outer (mm)
int jml_celah = 18;          // jumlah celah sensor
volatile unsigned int counter = 0;    // counter for sensor
unsigned int RPM = 0;        // Revolutions per minute
float speedbelt = 0;         // Belt speed (m/s)
volatile bool printFlag = false;
unsigned long previousMillis = 0;
void ICACHE_RAM_ATTR addcount();

void setup() {
  Serial.begin(9600);
  pinMode(proximity1, INPUT);
  pinMode(proximity2, INPUT);
  pinMode(sensorPin, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Terhubung ke WiFi
  Serial.println();
  Serial.println();
  Serial.print("Menghubungkan ke: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
  }

  Serial.println("");
  Serial.println("WiFi Terhubung");

  // Mencetak alamat IP
  Serial.print("Alamat IP: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  //RTC
  Wire.begin(14, 12);
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(_DATE), F(TIME_)));
  }

  // Mendapatkan nilai detectionCounter dari Firebase
  detectionCounter = firebase.getInt("detectionCounter");
  Serial.print("Received Int:");
  Serial.println(detectionCounter);
}

void loop() {
  int sensorValue1 = digitalRead(proximity1);
  int sensorValue2 = digitalRead(proximity2);
  String statusId = String(++lastStatusId);
   while (statusId.length() < 6) {
    statusId = "0" + statusId;
  }
  String graphId = String(++lastGraphId);
  while (graphId.length() < 6) {
    graphId = "0" + graphId;
  }
  String warning = "Warning: Material stuck for too long";
  unsigned long hitungKecepatan = millis();

      if (sensorValue1 != lastSensorValue1 || sensorValue2 != lastSensorValue2 ){
        String sensor1 = (sensorValue1 == LOW) ? "Object Detected" : "No Object Detected";
        String sensor2 = (sensorValue2 == LOW) ? "Object Detected" : "No Object Detected";
        firebase.setString("proximity1", sensor1);
        firebase.setString("proximity2", sensor2);
      }
      if (sensorValue1 == LOW) {
        Serial.println("Proximity1 Mendeteksi");
        startTime = millis();  // Start the timer when sensor1 is detected
      }
      if (startTime != 0 && sensorValue2 == LOW) {
        // firebase.setInt("/Status/" + statusId + "/acknowledge", 1);
        // firebase.setString("/Status/" + statusId + "/message", "Material Passed Through Both Sensors Succesfully");
        detectionCounter++;
        firebase.setInt("detectionCounter", detectionCounter);
        Serial.println("Proximity2 Mendeteksi");
        startTime = 0;  // Turn off the timer when sensor2 is detected
      }
      // Material Stuck
      if (startTime != 0 && millis() - startTime > 15000) {
        RTC();
        firebase.setInt("/Status/" + statusId + "/acknowledge", acknowledge);
        firebase.setString("/Status/" + statusId + "/message", warning);
        firebase.setString("/Status/" + statusId + "/timestamp", timestamp);
        startTime = 0;
      }
      // //sensor kecepatan
      Serial.println(hitungKecepatan);
      if(millis() - hitungKecepatan > 15000){
        beltvelocity();
        RPMcalc();   // Calculate RPM after the interrupt is detached
        BeltSpeed(); // Calculate BeltSpeed after the interrupt is detached
        RTC();
        firebase.setInt("RPM1", RPM);
        firebase.setFloat("beltSpeed1", speedbelt);
        firebase.setFloat("/Graph/" + graphId + "/beltSpeed1", speedbelt);
        firebase.setInt("/Graph/" + graphId + "/RPM1", RPM);
        firebase.setString("/Graph/" + graphId + "/timestamp", timestamp);
        hitungKecepatan = 0;
      }
      
}
// Measure belt speed
void beltvelocity() {
  counter = 0;
  attachInterrupt(digitalPinToInterrupt(sensorPin), addcount, CHANGE);
  delay(period);
  detachInterrupt(digitalPinToInterrupt(sensorPin));
}
void RPMcalc() {
  RPM = ((counter / jml_celah) * 60) / (period / 1000); // 60 = Calculate revolutions per minute (RPM)
}
void BeltSpeed() {
  speedbelt = ((2 * pi * radio * RPM) / 60) / 1000; // Calculate belt speed in m/s; 60 = convert minutes to seconds; 1000 = convert mm to m
}
void ICACHE_RAM_ATTR addcount() {
  counter++;
  printFlag = true;
}
void RTC() {
  DateTime now = rtc.now();
  timestamp = String(now.day(), DEC) + '/' +
             String(now.month(), DEC) + '/' +
             String(now.year(), DEC) + ' ' +
             String(now.hour(), DEC) + ':' +
             String(now.minute(), DEC) + ':' +
             String(now.second(), DEC);
}