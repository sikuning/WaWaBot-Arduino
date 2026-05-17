/**
 * Contoh: mulai sesi & ambil QR untuk pairing WhatsApp.
 *
 * 1. Upload sketch ini
 * 2. Buka Serial Monitor
 * 3. Salin URL/base64 QR dari respons (atau buka di browser jika isHtml=true)
 * 4. Di HP: WhatsApp > Pengaturan > Perangkat tertaut > Tautkan perangkat > scan QR
 * 5. Cek status dengan getSession() / getInfoMe()
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

  bot.begin();

  Serial.println("Memulai sesi...");
  if (!bot.start()) {
    Serial.print("start gagal: ");
    Serial.println(bot.lastHttpCode());
    Serial.println(bot.lastResponse());
    return;
  }
  Serial.println("start OK");
  delay(2000);

  Serial.println("Mengambil QR...");
  String qr = bot.getQR(false);
  if (qr.length() > 0) {
    Serial.println("=== QR Response ===");
    Serial.println(qr);
    Serial.println("Scan QR di WhatsApp > Pengaturan > Perangkat tertaut");
  } else {
    Serial.println("Gagal ambil QR");
    Serial.println(bot.lastError());
  }

  delay(5000);

  Serial.println("Cek sesi...");
  Serial.println(bot.getSession());

  Serial.println("Cek info-me...");
  Serial.println(bot.getInfoMe());
}

void loop() {}
