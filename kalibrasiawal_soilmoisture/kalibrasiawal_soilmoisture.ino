const int dry = 2992; // value for dry sensor
const int wet = 1097; // value for wet sensor


void setup() {
  Serial.begin(115200);
}

// void loop() {
//   Serial.println(analogRead(35)); //pin harus analog
//   delay(200);
// }
//nilai yang diambil rata2

void loop()
{
   int sensorVal = analogRead(35);
 
   int percentageHumididy = map(sensorVal, wet, dry, 84, 0); 
 
   Serial.print(percentageHumididy);
   Serial.println("%");
   
   delay(500);
}