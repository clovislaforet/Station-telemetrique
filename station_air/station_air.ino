/// Station télémétrique - station air
/// Clovis Laforêt et Clément Letranchant
/// FIMI 2A ENSIL-ENSCI

/// Code à flasher dans l'Arduino Nano Every


#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <VMA430_GPS.h>
#include <SoftwareSerial.h>

#define SEALEVELPRESSURE_HPA (1019) /// A mettre a jour (souvent 1013.25)

Adafruit_BME280 bme; // Creation objet BME280
SoftwareSerial ss(3, 4);
VMA430_GPS gps(&ss);
const int led_blanche = 7; // Pin de la led blanche
const int led_rouge = 6; // Pin de la led rouge
const int led_verte = 5; // Pin de la led verte

struct DonneesVol {
  float lat;
  float lon;
  float altitude;
  float pression;
  float temperature;
  float humidite;
};

void setup() {
  Serial.begin(115200);

  // LED d'état
  pinMode(led_blanche, OUTPUT);
  pinMode(led_verte, OUTPUT);
  pinMode(led_rouge, OUTPUT);
  digitalWrite(led_blanche, HIGH);
  delay(1000);

  // Démarrage BME
  if (!bme.begin(0x76)) {
    digitalWrite(led_rouge, HIGH);
    Serial.println("Erreur de detection BME280");
    while (1) { delay(10); }
    digitalWrite(led_rouge, LOW);
  }
  Serial.println("BME280 initialisé");
  digitalWrite(led_verte, HIGH);
  delay(500);
  digitalWrite(led_verte, LOW);
  delay(100);

  // Démarrage Lora
  while (!Serial);
  PORTMUX.TWISPIROUTEA = PORTMUX_SPI0_ALT2_gc; // association Arduino nano
  LoRa.setPins(8, 9, 2);
  if (!LoRa.begin(434E6)) {
    digitalWrite(led_rouge, HIGH);
    Serial.println("Erreur d'initialisation LoRa");
    while (1);
    digitalWrite(led_rouge, LOW);
  }
  Serial.println("LoRa initialisé");
  digitalWrite(led_verte, HIGH);
  delay(500);
  digitalWrite(led_verte, LOW);

  // Démarrage GPS
  gps.begin(9600);
  gps.setUBXNav();
  Serial.println("GPS initialisé");
  digitalWrite(led_verte, HIGH);
  delay(500);
  digitalWrite(led_verte, LOW);
  delay(100);

  delay(1000);
  digitalWrite(led_blanche, LOW);
  digitalWrite(led_verte, HIGH);
  Serial.println("Station air prete");
}

void loop() {

  // Lecture BME
  float temperature = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0F;
  float humidity = bme.readHumidity();
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA); // Fiable pour des variations de pression
  Serial.println("Temperature, pression, humidité et pression à jour");

  // Lecture GPS
  float lat = 0.0;
  float lon = 0.0;
  bool gps_valide = false;
  if (gps.getUBX_packet()) { // Vérifie si un paquet UBX valide a été reçu depuis le GPS
    gps.parse_ubx_data(); // Décodage des données reçues
    if (gps.location.latitude != 0.0 || gps.location.longitude != 0.0) {
      lat = gps.location.latitude;
      lon = gps.location.longitude;
      gps_valide = true;
    }
  }

  // Mise à jour des donnees
  DonneesVol data;
  data.lat         = gps_valide ? lat : 0.0;
  data.lon         = gps_valide ? lon : 0.0;
  data.altitude    = altitude;
  data.pression    = pressure;
  data.temperature = temperature;
  data.humidite    = humidity;

  // Emission
  LoRa.beginPacket();
  LoRa.write((uint8_t*)&data, sizeof(data));
  LoRa.endPacket();
  Serial.println("Envoyé vers station sol");

  // Contrôle
  Serial.print("GPS ");
  Serial.print(gps_valide ? "(fix) " : "(pas de fix) ");
  Serial.print(data.lat, 6); Serial.print(", "); Serial.println(data.lon, 6);
  Serial.print("Alt : "); Serial.print(data.altitude);
  Serial.print("  Temp : "); Serial.print(data.temperature);
  Serial.print("  Hum : "); Serial.print(data.humidite);
  Serial.print("  Pres : "); Serial.println(data.pression);
  Serial.println();

  Serial.println();
  delay(1000);
}