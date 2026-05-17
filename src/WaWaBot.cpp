#include "WaWaBot.h"

namespace {

String jsonEscape(const char* value) {
  String out = "\"";
  if (!value) {
    out += "\"";
    return out;
  }
  for (const char* p = value; *p; ++p) {
    const char c = *p;
    if (c == '\\' || c == '\"') {
      out += '\\';
      out += c;
    } else if (c == '\n') {
      out += "\\n";
    } else if (c == '\r') {
      out += "\\r";
    } else if (c == '\t') {
      out += "\\t";
    } else {
      out += c;
    }
  }
  out += "\"";
  return out;
}

}  // namespace

WaWaBot::WaWaBot(const char* apiKey, const char* accountId)
    : _apiKey(apiKey ? apiKey : ""),
      _accountId(accountId ? accountId : ""),
      _host(DEFAULT_HOST),
      _port(DEFAULT_PORT),
      _skipTlsVerify(true),
      _lastHttpCode(0) {}

void WaWaBot::begin(const char* host, uint16_t port, bool skipCertificateVerify) {
  _host = host ? host : DEFAULT_HOST;
  _port = port;
  _skipTlsVerify = skipCertificateVerify;
  prepareTls();
}

void WaWaBot::prepareTls() {
  if (_skipTlsVerify) {
    _client.setInsecure();
  }
}

String WaWaBot::accountPath(const char* suffix) const {
  return "/accounts/" + _accountId + suffix;
}

String WaWaBot::buildUrl(const String& path) const {
  String url = "https://";
  url += _host;
  if (_port != 443) {
    url += ":";
    url += _port;
  }
  url += path;
  return url;
}

bool WaWaBot::ensureWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }
  _lastError = "WiFi belum terhubung";
  return false;
}

bool WaWaBot::httpRequest(const char* method, const String& path,
                          const char* accept, const char* contentType,
                          const String& body) {
  _lastError = "";
  _lastResponse = "";
  _lastHttpCode = 0;

  if (!ensureWiFi()) {
    return false;
  }

  prepareTls();

  HTTPClient http;
  const String url = buildUrl(path);

  if (!http.begin(_client, url)) {
    _lastError = "Gagal membuka koneksi HTTP";
    return false;
  }

  http.addHeader("x-api-key", _apiKey);
  if (accept) {
    http.addHeader("accept", accept);
  }
  if (contentType) {
    http.addHeader("Content-Type", contentType);
  }

  if (strcmp(method, "GET") == 0) {
    _lastHttpCode = http.GET();
  } else if (strcmp(method, "POST") == 0) {
    _lastHttpCode = http.POST(body);
  } else {
    http.end();
    _lastError = "Metode HTTP tidak didukung";
    return false;
  }

  if (_lastHttpCode > 0) {
    _lastResponse = http.getString();
  } else {
    _lastError = http.errorToString(_lastHttpCode);
  }

  http.end();
  return _lastHttpCode >= 200 && _lastHttpCode < 300;
}

bool WaWaBot::httpMultipart(const String& path, const String& to,
                            const String& caption, const uint8_t* fileData,
                            size_t fileLen, const char* filename,
                            const char* mimeType) {
  _lastError = "";
  _lastResponse = "";
  _lastHttpCode = 0;

  if (!ensureWiFi()) {
    return false;
  }
  if (!fileData || fileLen == 0) {
    _lastError = "Data file kosong";
    return false;
  }

  prepareTls();

  const String boundary = "WaWaBot_" + String(millis(), HEX);

  const String partTo =
      "--" + boundary + "\r\n"
      "Content-Disposition: form-data; name=\"to\"\r\n\r\n" +
      to + "\r\n";

  const String partCaption =
      "--" + boundary + "\r\n"
      "Content-Disposition: form-data; name=\"caption\"\r\n\r\n" +
      caption + "\r\n";

  const String partFileHeader =
      "--" + boundary + "\r\n"
      "Content-Disposition: form-data; name=\"file\"; filename=\"" +
      String(filename) + "\"\r\n"
      "Content-Type: " +
      String(mimeType) + "\r\n\r\n";

  const String partEnd = "\r\n--" + boundary + "--\r\n";

  const size_t totalLen =
      partTo.length() + partCaption.length() + partFileHeader.length() +
      fileLen + partEnd.length();

  uint8_t* payload = static_cast<uint8_t*>(malloc(totalLen));
  if (!payload) {
    _lastError = "Gagal alokasi memori untuk multipart";
    return false;
  }

  size_t offset = 0;
  auto append = [&](const String& s) {
    memcpy(payload + offset, s.c_str(), s.length());
    offset += s.length();
  };

  append(partTo);
  append(partCaption);
  append(partFileHeader);
  memcpy(payload + offset, fileData, fileLen);
  offset += fileLen;
  append(partEnd);

  HTTPClient http;
  const String url = buildUrl(path);

  if (!http.begin(_client, url)) {
    free(payload);
    _lastError = "Gagal membuka koneksi HTTP";
    return false;
  }

  http.addHeader("x-api-key", _apiKey);
  http.addHeader("accept", "application/json");
  http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);

  _lastHttpCode = http.POST(payload, totalLen);
  free(payload);

  if (_lastHttpCode > 0) {
    _lastResponse = http.getString();
  } else {
    _lastError = http.errorToString(_lastHttpCode);
  }

  http.end();
  return _lastHttpCode >= 200 && _lastHttpCode < 300;
}

bool WaWaBot::start() {
  return httpRequest("POST", accountPath("/start"), "application/json", nullptr,
                     "");
}

String WaWaBot::getSession() {
  if (httpRequest("GET", accountPath("/session"))) {
    return _lastResponse;
  }
  return "";
}

String WaWaBot::getQR(bool isHtml) {
  String path = accountPath("/qr");
  path += "?ishtml=";
  path += isHtml ? "true" : "false";

  if (httpRequest("GET", path, "*/*")) {
    return _lastResponse;
  }
  return "";
}

String WaWaBot::getInfoMe() {
  if (httpRequest("GET", "/info-me")) {
    return _lastResponse;
  }
  return "";
}

bool WaWaBot::sendText(const char* to, const char* text) {
  String body = "{\"to\":";
  body += jsonEscape(to);
  body += ",\"text\":";
  body += jsonEscape(text);
  body += "}";

  return httpRequest("POST", accountPath("/send"), "application/json",
                     "application/json", body);
}

bool WaWaBot::sendMedia(const char* to, const char* caption,
                        const uint8_t* fileData, size_t fileLen,
                        const char* filename, const char* mimeType) {
  return httpMultipart(accountPath("/send-media"), to ? to : "",
                       caption ? caption : "", fileData, fileLen,
                       filename ? filename : "image.jpg", mimeType);
}

String WaWaBot::lookup(const char* to) {
  String path = accountPath("/lookup");
  path += "?to=";
  path += to ? to : "";

  if (httpRequest("GET", path)) {
    return _lastResponse;
  }
  return "";
}

bool WaWaBot::logout() {
  return httpRequest("POST", accountPath("/logout"), "application/json",
                     nullptr, "");
}
