/// Station télémétrique - station sol
/// Clovis Laforêt et Clément Letranchant
/// FIMI 2A ENSIL-ENSCI

/// Code à flasher dans l'Arduino Mega 2560


#include <SPI.h>
#include <LoRa.h>

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

  // Démarrage Lora
  while (!Serial);
  LoRa.setPins(53, 9, 2);
  if (!LoRa.begin(434E6)) {
    Serial.println("LoRa init failed!");
    while (1);}
  Serial.println("Station sol prete");

  delay(1000);
}

void loop() {

  // Réception télémétrie
  int packetSize = LoRa.parsePacket();
  if (packetSize == sizeof(DonneesVol)) {

    // Réception
    DonneesVol data;
    uint8_t* ptr = (uint8_t*)&data;
    for (int i = 0; i < sizeof(DonneesVol); i++) {
      ptr[i] = LoRa.read();}
    Serial.print("GPS : "); Serial.print(data.lat, 6); Serial.print(", "); Serial.println(data.lon, 6);
    Serial.print("Altitude : "); Serial.println(data.altitude);
    Serial.print("Pression : "); Serial.println(data.pression);
    Serial.print("Temp : "); Serial.println(data.temperature);
    Serial.print("Humidité : "); Serial.println(data.humidite);
    Serial.print("RSSI : "); Serial.println(LoRa.packetRssi());
    Serial.println("");
  }
  
  delay(100);
}