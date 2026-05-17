/**
 * Contoh: kirim pesan teks WhatsApp via WaWaBot API.
 *
 * Ganti WIFI_SSID, WIFI_PASS, API_KEY, dan ACCOUNT_ID sebelum upload.
 * Pastikan akun sudah di-pair (lihat contoh LinkDevice).
 */

#include <WiFi.h>
#include <WaWaBot.h>

const char* WIFI_SSID = "your-wifi";
const char* WIFI_PASS = "your-password";
const char* API_KEY = "your-api-key";
const char* ACCOUNT_ID = "user-1";

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

  bot.begin();  // host default: https://wa-api.knn.my.id

  if (bot.sendText("6281234567890", "Halo dari Arduino WaWaBot!")) {
    Serial.println("Pesan terkirim");
    Serial.println(bot.lastResponse());
  } else {
    Serial.print("Gagal, HTTP ");
    Serial.println(bot.lastHttpCode());
    Serial.println(bot.lastError());
    Serial.println(bot.lastResponse());
  }
}

void loop() {}
