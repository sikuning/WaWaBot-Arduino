#ifndef WAWABOT_H
#define WAWABOT_H

#include <Arduino.h>

#if defined(ESP32)
  #include <WiFi.h>
  #include <WiFiClientSecure.h>
  #include <HTTPClient.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiClientSecure.h>
  #include <ESP8266HTTPClient.h>
#else
  #error "WaWaBot membutuhkan board ESP32 atau ESP8266 dengan WiFi"
#endif

/**
 * Client HTTP untuk API WaWaBot (https://wa-api.knn.my.id).
 *
 * Alur pairing:
 *   1. start()           -> POST /accounts/{id}/start
 *   2. getQR()           -> GET  /accounts/{id}/qr
 *   3. Scan QR di HP (WhatsApp > Pengaturan > Perangkat tertaut)
 *   4. getSession()      -> cek status login
 *   5. sendText() / sendMedia()
 */
class WaWaBot {
public:
  static constexpr const char* DEFAULT_HOST = "wa-api.knn.my.id";
  static constexpr uint16_t DEFAULT_PORT = 443;

  WaWaBot(const char* apiKey, const char* accountId);

  /** Atur host API (default: wa-api.knn.my.id:443). */
  void begin(const char* host = DEFAULT_HOST, uint16_t port = DEFAULT_PORT,
             bool skipCertificateVerify = true);

  bool start();
  String getSession();
  String getQR(bool isHtml = false);
  String getInfoMe();
  bool sendText(const char* to, const char* text);
  bool sendMedia(const char* to, const char* caption,
                 const uint8_t* fileData, size_t fileLen,
                 const char* filename,
                 const char* mimeType = "image/jpeg");
  String lookup(const char* to);
  bool logout();

  int lastHttpCode() const { return _lastHttpCode; }
  const String& lastResponse() const { return _lastResponse; }
  const String& lastError() const { return _lastError; }

private:
  String _apiKey;
  String _accountId;
  String _host;
  uint16_t _port;
  bool _skipTlsVerify;

  int _lastHttpCode;
  String _lastResponse;
  String _lastError;

  WiFiClientSecure _client;

  String accountPath(const char* suffix) const;
  String buildUrl(const String& path) const;
  void prepareTls();
  bool ensureWiFi();
  bool httpRequest(const char* method, const String& path,
                   const char* accept = "application/json",
                   const char* contentType = nullptr,
                   const String& body = String());
  bool httpMultipart(const String& path, const String& to, const String& caption,
                     const uint8_t* fileData, size_t fileLen,
                     const char* filename, const char* mimeType);
};

#endif
