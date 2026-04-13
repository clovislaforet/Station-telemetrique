/// Station télémétrique - station air
/// Clovis Laforêt et Clément Letranchant
/// FIMI 2A ENSIL-ENSCI

/// Code à flasher dans l'ESP32-S3 IA CAM


#include "esp_camera.h"   // contrôle caméra ESP32
#include "FS.h"           // système de fichiers
#include <SD.h>           // carte SD en SPI
#include <SPI.h>          // protocole SPI

// Pins camera
#define PWDN_GPIO_NUM  -1  // mise en veille (-1 = non utilisé)
#define RESET_GPIO_NUM -1  // reset (-1 = non utilisé)
#define XCLK_GPIO_NUM   5  // horloge capteur
#define Y9_GPIO_NUM     4
#define Y8_GPIO_NUM     6
#define Y7_GPIO_NUM     7
#define Y6_GPIO_NUM    14
#define Y5_GPIO_NUM    17
#define Y4_GPIO_NUM    21
#define Y3_GPIO_NUM    18
#define Y2_GPIO_NUM    16
#define VSYNC_GPIO_NUM  1  // synchro verticale
#define HREF_GPIO_NUM   2  // synchro horizontale
#define PCLK_GPIO_NUM  15  // horloge pixel
#define SIOD_GPIO_NUM   8  // I2C SDA
#define SIOC_GPIO_NUM   9  // I2C SCL

// Pins SD SPI
#define SD_CS   10
#define SD_MOSI 11
#define SD_SCLK 12
#define SD_MISO 13

// Réglages qualité
#define CAPTURE_INTERVAL_MS 100     // délai entre captures (ms) — 100=10FPS, 200=5FPS
#define FRAME_SIZE  FRAMESIZE_SVGA  // SVGA=800x600 | VGA=640x480 | XGA=1024x768
#define JPEG_QUALITY 8              // 0-63 : plus petit = meilleure qualité (8 = très bon)

int imageCounter = 0;
String recordFolder = "";

// Retourne le prochain dossier disponible : /record, /record1, /record2...
String getNewRecordFolder() {
  if (!SD.exists("/record")) return "/record";
  for (int i = 1; i < 1000; i++) {
    String name = "/record" + String(i);
    if (!SD.exists(name)) return name;
  }
  return "/record_overflow";
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== DFR1154 Camera + SD ===");

  // Init SD — bus SPI à 40MHz pour accélérer les écritures
  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, SPI, 40000000)) { Serial.println("SD Mount Failed"); return; }
  Serial.printf("SD OK — %llu MB\n", SD.cardSize() / (1024 * 1024));

  // Création du dossier de session (nouveau à chaque démarrage)
  recordFolder = getNewRecordFolder();
  SD.mkdir(recordFolder);
  Serial.println("Dossier : " + recordFolder);

  // Init caméra
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk  = XCLK_GPIO_NUM;  config.pin_pclk  = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;  config.pin_href  = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn  = PWDN_GPIO_NUM;   config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // Résolution selon PSRAM disponible
  if (psramFound()) {
    config.frame_size   = FRAME_SIZE;
    config.jpeg_quality = JPEG_QUALITY;
    config.fb_count     = 2;  // double buffer : capture pendant l'écriture
    Serial.println("PSRAM détecté");
  } else {
    config.frame_size   = FRAMESIZE_VGA;  // sans PSRAM on reste en VGA max
    config.jpeg_quality = 12;
    config.fb_count     = 1;
    Serial.println("Pas de PSRAM — forcé VGA");
  }

  if (esp_camera_init(&config) != ESP_OK) { Serial.println("Camera init failed"); return; }

  // Réglages capteur
  sensor_t *s = esp_camera_sensor_get();
  s->set_vflip(s, 1);          // rotation 180°
  s->set_hmirror(s, 0);
  s->set_brightness(s, 0);     // luminosité (-2 à +2)
  s->set_contrast(s, 0);       // contraste (-2 à +2)
  s->set_saturation(s, 0);     // saturation (-2 à +2)
  s->set_exposure_ctrl(s, 1);  // auto exposition
  s->set_gain_ctrl(s, 1);      // auto gain (ISO)

  Serial.printf("Camera OK — %s q=%d — Enregistrement...\n",
    (FRAME_SIZE == FRAMESIZE_SVGA) ? "SVGA 800x600" :
    (FRAME_SIZE == FRAMESIZE_VGA)  ? "VGA 640x480"  :
    (FRAME_SIZE == FRAMESIZE_XGA)  ? "XGA 1024x768" : "autre",
    JPEG_QUALITY);
}

void loop() {
  unsigned long t0 = millis();

  camera_fb_t *fb = esp_camera_fb_get();  // capture JPEG
  if (!fb) { Serial.println("Capture failed"); delay(500); return; }

  String path = recordFolder + "/img_" + String(imageCounter) + ".jpg";
  File file = SD.open(path.c_str(), FILE_WRITE);

  if (!file) {
    Serial.println("Echec : " + path);
  } else {
    file.write(fb->buf, fb->len);
    file.close();
    unsigned long elapsed = millis() - t0;
    Serial.printf("Saved: %s (%u bytes) — %lums\n", path.c_str(), fb->len, elapsed);
    imageCounter++;
  }

  esp_camera_fb_return(fb);  // libération mémoire

  // Attendre le reste de l'intervalle (si l'écriture a pris moins de temps)
  long wait = CAPTURE_INTERVAL_MS - (long)(millis() - t0);
  if (wait > 0) delay(wait);
}
