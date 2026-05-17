# WaWaBot

Library Arduino untuk mengirim pesan WhatsApp dari **ESP32** atau **ESP8266** melalui [WaWaBot API](https://wa-api.knn.my.id).

Cukup install library, hubungkan WiFi, lalu panggil class `WaWaBot` untuk pairing perangkat, kirim teks, atau kirim gambar.

## Persyaratan

| Item | Keterangan |
|------|------------|
| Board | ESP32 atau ESP8266 dengan WiFi |
| Arduino IDE | 1.8.x atau 2.x (opsional) |
| PlatformIO | VS Code + extension PlatformIO (opsional) |
| Kredensial | `apiKey` dan `accountId` dari penyedia layanan WaWaBot |
| Koneksi | WiFi aktif sebelum memanggil method library |

## Instalasi

### Arduino IDE (manual)

1. Download atau clone repositori ini.
2. Salin folder `WaWaBot-Arduino` ke folder libraries Arduino:
   - **macOS:** `~/Documents/Arduino/libraries/`
   - **Windows:** `Documents\Arduino\libraries\`
   - **Linux:** `~/Arduino/libraries/`
3. Restart Arduino IDE.
4. Pilih board **ESP32** atau **ESP8266**, lalu buka contoh dari menu **File → Examples → WaWaBot**.

### Arduino IDE (ZIP)

1. **Sketch → Include Library → Add .ZIP Library...**
2. Pilih file ZIP dari repositori ini.

### Arduino Library Manager (setelah terdaftar)

Setelah library masuk ke [Arduino Library Registry](https://github.com/arduino/library-registry), pengguna bisa install langsung dari IDE:

1. Buka **Sketch → Include Library → Manage Libraries...**
2. Cari **`WaWaBot`**
3. Klik **Install**

Atau lewat Arduino CLI:

```bash
arduino-cli lib install WaWaBot
```

### PlatformIO

Library yang **sama** bisa dipakai di PlatformIO. API dan class `WaWaBot` identik dengan Arduino IDE — bedanya hanya cara install (lewat `platformio.ini`).

#### Install via `lib_deps` (disarankan)

Tambahkan di `platformio.ini`:

```ini
lib_deps =
    https://github.com/sikuning/WaWaBot-Arduino.git#1.0.0
```

Versi terbaru dari branch `main` (tanpa pin tag):

```ini
lib_deps =
    https://github.com/sikuning/WaWaBot-Arduino.git
```

#### Contoh `platformio.ini` — ESP32

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200

lib_deps =
    https://github.com/sikuning/WaWaBot-Arduino.git#1.0.0
```

#### Contoh `platformio.ini` — ESP8266

```ini
[env:nodemcuv2]
platform = espressif8266
board = nodemcuv2
framework = arduino
monitor_speed = 115200

lib_deps =
    https://github.com/sikuning/WaWaBot-Arduino.git#1.0.0
```

#### Contoh kode (`src/main.cpp`)

```cpp
#include <Arduino.h>
#include <WiFi.h>
#include <WaWaBot.h>

const char* WIFI_SSID = "nama-wifi";
const char* WIFI_PASS = "password-wifi";
const char* API_KEY = "api-key-anda";
const char* ACCOUNT_ID = "user-1";

WaWaBot bot(API_KEY, ACCOUNT_ID);

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  bot.begin();

  if (bot.sendText("6281234567890", "Halo dari PlatformIO!")) {
    Serial.println(bot.lastResponse());
  } else {
    Serial.print("HTTP ");
    Serial.println(bot.lastHttpCode());
    Serial.println(bot.lastError());
  }
}

void loop() {}
```

#### Perintah CLI

```bash
pio pkg install          # install lib_deps
pio run                  # compile
pio run -t upload        # upload ke board
pio device monitor       # serial monitor
```

#### Struktur project PlatformIO

```text
my-wawabot-project/
├── platformio.ini
└── src/
    └── main.cpp
```

Contoh sketch Arduino ada di folder `examples/` repo ini. Salin logika ke `src/main.cpp`, atau buka folder `examples/SendText/` sebagai referensi.

#### Install manual (opsional)

Salin folder library ke `lib/WaWaBot/` di root project PlatformIO:

```text
my-project/
├── lib/
│   └── WaWaBot/          ← isi repo library (src/, library.properties, ...)
├── platformio.ini
└── src/
    └── main.cpp
```

#### Arduino IDE vs PlatformIO

| | Arduino IDE | PlatformIO |
|---|-------------|------------|
| Install library | Library Manager / ZIP / folder `libraries` | `lib_deps` di `platformio.ini` |
| Konfigurasi board | Menu Tools | `board` di `platformio.ini` |
| Kode utama | `*.ino` di root sketch | `src/main.cpp` |
| Class & API | `WaWaBot` | `WaWaBot` (sama) |

> **Catatan:** Library Manager Arduino dan registry PlatformIO terpisah. Setelah WaWaBot terdaftar di Arduino Library Manager, di PlatformIO tetap paling mudah memakai URL GitHub di `lib_deps`.

## Publikasi ke Arduino Library Manager

Agar library bisa di-install lewat **Library Manager**, Anda harus mendaftarkannya ke index resmi Arduino. Library Manager **tidak** membaca repo GitHub secara otomatis — perlu submit sekali ke registry Arduino.

### Checklist sebelum submit

| # | Syarat | Status WaWaBot |
|---|--------|----------------|
| 1 | Repo publik di GitHub / GitLab / Bitbucket | Push ke GitHub dulu |
| 2 | `library.properties` di **root** repo | Sudah ada |
| 3 | Nama unik di index (`name=WaWaBot`) | Cek belum dipakai library lain |
| 4 | Struktur sesuai [Arduino Library Specification](https://arduino.github.io/arduino-cli/latest/library-specification/) | `src/`, `examples/` |
| 5 | **Git tag** semver (mis. `1.0.0`) yang cocok dengan `version` di `library.properties` | Buat tag (lihat bawah) |
| 6 | Tag tersebut sudah compliant saat dibuat | Commit + tag setelah repo rapi |
| 7 | Tidak ada file `.exe`, `.development`, symlink | — |

### Langkah 1 — Push ke GitHub

```bash
cd WaWaBot-Arduino
git init
git add .
git commit -m "Release v1.0.0"
git remote add origin https://github.com/robbydwihartanto/WaWaBot-Arduino.git
git branch -M main
git push -u origin main
```

Pastikan URL di `library.properties` sama dengan repo GitHub:

```properties
url=https://github.com/robbydwihartanto/WaWaBot-Arduino
```

### Langkah 2 — Buat Git tag (wajib)

Library Manager hanya mengindeks **release/tag**, bukan branch `main` saja.

```bash
# version di library.properties harus sama: 1.0.0
git tag 1.0.0
git push origin 1.0.0
```

Atau lewat GitHub: **Releases → Create a new release** → tag `1.0.0`.

### Langkah 3 — (Opsional) Cek compliance dengan arduino-lint

```bash
# Install arduino-lint: https://github.com/arduino/arduino-lint
arduino-lint --library-manager update
```

Perbaiki error yang muncul sebelum submit.

### Langkah 4 — Daftar ke Arduino Library Registry

1. Fork repo [arduino/library-registry](https://github.com/arduino/library-registry)
2. Edit file [`repositories.txt`](https://github.com/arduino/library-registry/blob/main/repositories.txt)
3. Tambahkan **satu baris** URL repo Anda (urutan bebas):

   ```text
   https://github.com/robbydwihartanto/WaWaBot-Arduino
   ```

4. Commit di branch baru → buka **Pull Request** ke `arduino/library-registry`
5. Pantau PR — bot **@ArduinoBot** akan mengecek library; perbaiki jika ada error
6. Setelah PR **merged**, tunggu ±1 hari — library muncul di Library Manager

Panduan resmi lengkap: [arduino/library-registry — Adding a library](https://github.com/arduino/library-registry#adding-a-library-to-library-manager)

### Langkah 5 — Update versi berikutnya

Setiap rilis baru:

1. Ubah `version` di `library.properties` (mis. `1.0.1`)
2. Commit perubahan
3. Buat tag baru (`1.0.1`) dan push
4. Indexer Arduino otomatis mendeteksi tag baru (±1 jam), **tanpa** PR registry lagi

Cek log indexer library Anda:

```text
http://downloads.arduino.cc/libraries/logs/github.com/robbydwihartanto/WaWaBot-Arduino/
```

### Ringkasan alur

```mermaid
flowchart LR
  A[Repo GitHub + library.properties] --> B[Git tag 1.0.0]
  B --> C[PR ke arduino/library-registry]
  C --> D[PR merged]
  D --> E[Muncul di Library Manager]
```

## Konfigurasi API

Library memakai host resmi berikut (HTTPS):

```
https://wa-api.knn.my.id
```

Setiap request mengirim header:

```
x-api-key: <API_KEY_ANDA>
```

`accountId` biasanya berupa string seperti `user-1` dan dipakai di path URL: `/accounts/{accountId}/...`.

## Alur pairing (hubungkan WhatsApp)

```
1. Terima apiKey & accountId dari penyedia layanan
2. start()      → POST /accounts/{id}/start
3. getQR()      → GET  /accounts/{id}/qr
4. Scan QR di HP:
   WhatsApp → Pengaturan (⚙️) → Perangkat tertaut → Tautkan perangkat
5. getSession() / getInfoMe() → cek nomor & status login
6. sendText() / sendMedia()   → kirim pesan
```

## Quick start

### Kirim pesan teks

```cpp
#include <WiFi.h>
#include <WaWaBot.h>

const char* WIFI_SSID = "nama-wifi";
const char* WIFI_PASS = "password-wifi";
const char* API_KEY   = "api-key-anda";
const char* ACCOUNT_ID = "user-1";

WaWaBot bot(API_KEY, ACCOUNT_ID);

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  bot.begin();  // default host: wa-api.knn.my.id

  if (bot.sendText("6281234567890", "Halo dari Arduino!")) {
    Serial.println("Berhasil");
    Serial.println(bot.lastResponse());
  } else {
    Serial.println(bot.lastError());
  }
}

void loop() {}
```

### Pairing perangkat (QR)

```cpp
bot.begin();

bot.start();
delay(2000);

String qr = bot.getQR(false);
Serial.println(qr);  // tampilkan / proses respons QR

// setelah scan di HP:
Serial.println(bot.getSession());
Serial.println(bot.getInfoMe());
```

### Kirim gambar

```cpp
// fileData = buffer berisi byte gambar (JPEG, PNG, dll.)
bot.sendMedia(
  "6281234567890",
  "Caption pesan",
  fileData,
  fileLen,
  "photo.jpg",
  "image/jpeg"
);
```

Lihat contoh lengkap di folder `examples/`:

| Contoh | Deskripsi |
|--------|-----------|
| `LinkDevice` | Start sesi, ambil QR, cek status |
| `SendText` | Kirim pesan teks |
| `SendMedia` | Kirim gambar dari SPIFFS |

## Referensi API (class `WaWaBot`)

### Konstruktor & setup

```cpp
WaWaBot bot("api-key", "user-1");
bot.begin();  // host default, port 443, skip TLS verify = true
bot.begin("wa-api.knn.my.id", 443, true);
```

### Method

| Method | HTTP | Deskripsi |
|--------|------|-----------|
| `start()` | `POST /accounts/{id}/start` | Mulai sesi WhatsApp |
| `getSession()` | `GET /accounts/{id}/session` | Cek status sesi (JSON) |
| `getQR(isHtml)` | `GET /accounts/{id}/qr?ishtml=...` | Ambil data QR |
| `getInfoMe()` | `GET /info-me` | Info akun / login |
| `sendText(to, text)` | `POST /accounts/{id}/send` | Kirim pesan teks |
| `sendMedia(...)` | `POST /accounts/{id}/send-media` | Kirim gambar/file |
| `lookup(to)` | `GET /accounts/{id}/lookup?to=...` | Cek nomor WhatsApp |
| `logout()` | `POST /accounts/{id}/logout` | Logout sesi |

### Debug & error handling

```cpp
bot.sendText("628xxx", "test");

int code = bot.lastHttpCode();    // kode HTTP (200, 401, 500, ...)
String body = bot.lastResponse(); // body respons server
String err  = bot.lastError();    // pesan error client (WiFi, TLS, dll.)
```

Method mengembalikan `true` jika HTTP status **2xx**, selain itu `false`.

## Format nomor telepon

Gunakan nomor internasional tanpa tanda `+`, contoh:

- `6281234567890` ✅
- `+6281234567890` (hindari `+` di JSON)

## Endpoint yang dipanggil library

Base URL: `https://wa-api.knn.my.id`

```
POST   /accounts/{accountId}/start
GET    /accounts/{accountId}/session
GET    /accounts/{accountId}/qr?ishtml=false
GET    /info-me
POST   /accounts/{accountId}/send
POST   /accounts/{accountId}/send-media
GET    /accounts/{accountId}/lookup?to=628xxx
POST   /accounts/{accountId}/logout
```

## TLS / HTTPS

Secara default library memakai `WiFiClientSecure` dengan **certificate verification dinonaktifkan** (`setInsecure()`), agar mudah dipakai di development.

Untuk production, pertimbangkan memasang root CA certificate di firmware dan set `skipCertificateVerify` ke `false` di `begin()`.

## Troubleshooting

| Masalah | Solusi |
|---------|--------|
| `WiFi belum terhubung` | Pastikan `WiFi.begin()` sukses sebelum memanggil `WaWaBot` |
| HTTP 401 / 403 | Periksa `apiKey` dan `accountId` |
| Kirim gagal padahal WiFi OK | Cek apakah akun sudah di-pair (`getSession()`) |
| QR tidak muncul | Panggil `start()` dulu, tunggu 1–2 detik, lalu `getQR()` |
| `sendMedia` gagal | Pastikan buffer file valid dan ukuran memori ESP cukup |

## Kontribusi

Issue dan pull request dipersilakan di [GitHub](https://github.com/robbydwihartanto/WaWaBot-Arduino).
