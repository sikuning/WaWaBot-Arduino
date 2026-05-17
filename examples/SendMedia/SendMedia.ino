/**
 * Contoh: kirim gambar WhatsApp via WaWaBot API.
 *
 * Ganti path file di SPIFFS/LittleFS sesuai board Anda,
 * atau ganti dengan data gambar dari SD card.
 */

#include <WiFi.h>
#include <FS.h>
#include <WaWaBot.h>

const char* WIFI_SSID = "your-wifi";
const char* WIFI_PASS = "your-password";
const char* API_KEY = "your-api-key";
const char* ACCOUNT_ID = "user-1";

// Simpan file gambar di flash (SPIFFS/LittleFS) dengan nama ini
const char* IMAGE_PATH = "/photo.jpg";

WaWaBot bot(API_KEY, ACCOUNT_ID);

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi OK");

#if defined(ESP32)
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount gagal");
    return;
  }
#elif defined(ESP8266)
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS mount gagal");
    return;
  }
#endif

  File file = SPIFFS.open(IMAGE_PATH, "r");
  if (!file) {
    Serial.println("File gambar tidak ditemukan di SPIFFS");
    return;
  }

  const size_t fileSize = file.size();
  uint8_t* buffer = static_cast<uint8_t*>(malloc(fileSize));
  if (!buffer) {
    Serial.println("Gagal alokasi buffer");
    file.close();
    return;
  }

  file.read(buffer, fileSize);
  file.close();

  bot.begin();

  if (bot.sendMedia("6281234567890", "Foto dari Arduino",
                    buffer, fileSize, "photo.jpg", "image/jpeg")) {
    Serial.println("Media terkirim");
    Serial.println(bot.lastResponse());
  } else {
    Serial.print("Gagal, HTTP ");
    Serial.println(bot.lastHttpCode());
    Serial.println(bot.lastError());
    Serial.println(bot.lastResponse());
  }

  free(buffer);
}

void loop() {}
