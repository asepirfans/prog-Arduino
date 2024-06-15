const int relayPin1 = 8; // Pin untuk relay 1
const int relayPin2 = 9; // Pin untuk relay 2
const int relayPin3 = 10; // Pin untuk relay 3

void setup() {
  Serial.begin(115200); // Komunikasi serial dengan ESP32
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  digitalWrite(relayPin1, LOW); // Memastikan relay mati pada awalnya
  digitalWrite(relayPin2, LOW); // Memastikan relay mati pada awalnya
  digitalWrite(relayPin3, LOW); // Memastikan relay mati pada awalnya
}

void loop() {
  if (Serial.available() > 0) { // Jika data tersedia dari ESP32
    String data = Serial.readStringUntil('\n'); // Baca data sampai baris baru

    if (data.startsWith("SOIL1:")) {
      int percentageHumidity1 = data.substring(6).toInt(); // Ambil nilai kelembaban
      Serial.print("Soil 1: ");
      Serial.println(percentageHumidity1);

      // Logika untuk mengontrol relay 1
      if (percentageHumidity1 < 50) { // Jika kelembaban di bawah 50%
        digitalWrite(relayPin1, HIGH); // Nyalakan relay 1 (pompa 1 aktif)
        Serial.println("Pompa 1 dinyalakan");
      } else {
        digitalWrite(relayPin1, LOW); // Matikan relay 1 (pompa 1 mati)
        Serial.println("Pompa 1 dimatikan");
      }
    }

    if (data.startsWith("SOIL2:")) {
      int percentageHumidity2 = data.substring(6).toInt(); // Ambil nilai kelembaban
      Serial.print("Soil 2: ");
      Serial.println(percentageHumidity2);

      // Logika untuk mengontrol relay 2
      if (percentageHumidity2 < 50) { // Jika kelembaban di bawah 50%
        digitalWrite(relayPin2, HIGH); // Nyalakan relay 2 (pompa 2 aktif)
        Serial.println("Pompa 2 dinyalakan");
      } else {
        digitalWrite(relayPin2, LOW); // Matikan relay 2 (pompa 2 mati)
        Serial.println("Pompa 2 dimatikan");
      }
    }

    if (data.startsWith("SOIL3:")) {
      int percentageHumidity3 = data.substring(6).toInt(); // Ambil nilai kelembaban
      Serial.print("Soil 3: ");
      Serial.println(percentageHumidity3);

      // Logika untuk mengontrol relay 3
      if (percentageHumidity3 < 50) { // Jika kelembaban di bawah 30%
        digitalWrite(relayPin3, HIGH); // Nyalakan relay 3 (pompa 3 aktif)
        Serial.println("Pompa 3 dinyalakan");
      } else {
        digitalWrite(relayPin3, LOW); // Matikan relay 3 (pompa 3 mati)
        Serial.println("Pompa 3 dimatikan");
      }
    }
  }
}
