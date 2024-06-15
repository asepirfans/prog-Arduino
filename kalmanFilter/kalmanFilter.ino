#include <SimpleKalmanFilter.h>

// Inisialisasi Kalman filter (measure_noise, estimate_noise, process_noise)
SimpleKalmanFilter soilMoistureKalmanFilter(2, 2, 0.01);

const int sensorPin = 34;
const int wet = 1097; // Gantilah dengan nilai sensor untuk kondisi tanah basah
const int dry = 2992; // Gantilah dengan nilai sensor untuk kondisi tanah kering
#define SOILPIN 35
const int bitADC = 4095;
float soilMoisture;
float mois;

void setup() {
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);
  pinMode(SOILPIN, INPUT);
}

void loop() {
  float adcValue = analogRead(SOILPIN);
  float filteredValue = soilMoistureKalmanFilter.updateEstimate(adcValue);
  soilMoisture = (100 - ((filteredValue / bitADC) * 100));
  mois = round(soilMoisture * 10.0) / 10.0;
  Serial.println("Sensor Soil:");
  Serial.print("Moisture Kalman: ");
  Serial.print(mois);
  Serial.println("%    ");

  int sensorVal = analogRead(sensorPin);
  float filteredSensorVal = soilMoistureKalmanFilter.updateEstimate(sensorVal);
  
  int percentageHumidity = map(filteredSensorVal, wet, dry, 84, 0);
  
  Serial.print(percentageHumidity);
  Serial.println("%");
  
  delay(500);
}
