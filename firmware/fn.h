#include <WiFiClientSecure.h>

// SECURITY: set this to match the "secret" value checked in code.gs (doPost()).
// Do NOT commit your real token to a public repository -- generate a new
// random token for your own deployment and keep it out of version control
// (e.g. via a gitignored secrets.h, or an environment-specific #define).
#define TOKEN_SECRET "YOUR_SECRET_TOKEN_HERE"

void initSHT(){
 Wire.begin(22, 21); 

  Serial.println("SHT40 Test");

  if (!sht40.begin(&Wire)) {  
    Serial.println("Couldn't find SHT40 sensor!");
    while (1) delay(1);
  }

  sht40.setPrecision(SHT4X_HIGH_PRECISION);


  Serial.println("SHT40 initialized.");
}

String buildJson(char* tmp, char* hum, const struct tm &timeinfo)
{
  char timeStr[10];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
  char dateStr[11];
  strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &timeinfo);

  String json = "{";
  json += "\"temperature\": ";
  json += tmp;
  json += ",\"humidity\": ";
  json += hum;
  json += ",\"deviceID\": ";
  json += DevID;
  json += ",";
  json += "\"timestamp\":\"";
  json += dateStr;
  json += "T";
  json += timeStr;
  json += "\","; //2026-02-26T11:32:22\",";
  json += "\"token\":\"" TOKEN_SECRET "\"";
  json += "}";
  return json;
}

// Low-level: POST one already-built JSON string. Returns true only on a
// confirmed 200/302 response from the server.
bool sendJson(const String &json)
{
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected");
    serverStat(SRV_DOWN);
    return false;
  }
  else{
    HTTPClient http;
    //http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(json);
    String response = http.getString();
    http.end();

    Serial.print("Response code: ");
    Serial.println(httpResponseCode);
    Serial.print("Response: ");
    Serial.println(response);
    Serial.print("Json: ");
    Serial.println(json);

    if (httpResponseCode > 0){
      if (httpResponseCode == 200 || httpResponseCode == 302) {
        serverStat(SRV_OK);
        return true;
      }
      else{
        serverStat(SRV_NOK);
        Serial.print("Error in sending POST: ");
        Serial.println(httpResponseCode);
        return false;
      }
    }
    else{
      serverStat(SRV_DOWN);
      return false;
    }
  

  //return false;
  }
}

// Append one JSON reading to the offline queue file on flash.
void queueReading(const String &json)
{
  File f = LittleFS.open(QUEUE_FILE, FILE_APPEND);
  if (!f) {
    Serial.println("Gagal membuka queue file untuk menyimpan data offline!");
    return;
  }
  if (f.size() >= QUEUE_MAX_BYTES) {
    Serial.println("Queue offline penuh — data ini TIDAK disimpan. Periksa koneksi/server.");
    f.close();
    return;
  }
  f.println(json);
  f.close();
  Serial.println("Internet/server tidak tersedia — data disimpan ke queue offline.");
}

// Try to send the single oldest entry in the offline queue. On success,
// that entry is removed from the file (queue may still have more items
// left — this function is called once per loop tick so a backlog drains
// gradually rather than all at once).
bool flushOneQueuedReading()
{
  //Serial.println("Flushing...");

  if (!LittleFS.exists(QUEUE_FILE))
  {
    Serial.print("-");
    return false;
  }

  // ============================================================
  // 1. OPEN QUEUE
  // ============================================================
  File src = LittleFS.open(QUEUE_FILE, FILE_READ);

  if (!src)
  {
    Serial.println("Gagal membuka queue.");
    return false;
  }

  if (src.size() == 0)
  {
    Serial.print(".");
    src.close();
    return false;
  }

  // ============================================================
  // 2. READ FIRST RECORD
  // ============================================================
  String firstLine = src.readStringUntil('\n');
  firstLine.trim();

  src.close();

  Serial.print("Record pertama: ");
  Serial.println(firstLine);

  if (firstLine.length() == 0)
  {
    Serial.println("Record pertama kosong.");
    return false;
  }

  // ============================================================
  // 3. SEND FIRST RECORD
  // ============================================================
  if (!sendJson(firstLine))
  {
    Serial.println("Gagal kirim.");
    return false;
  }

  Serial.println("Mengirim berhasil");


  // ============================================================
  // 4. READ REMAINING QUEUE INTO RAM
  //
  // IMPORTANT:
  // Tidak ada dst yang terbuka di sini.
  // ============================================================
  String rest = "";

  src = LittleFS.open(QUEUE_FILE, FILE_READ);

  if (!src)
  {
    Serial.println("Gagal membuka queue untuk pembacaan kedua.");
    return false;
  }

  // Skip first record
  src.readStringUntil('\n');

  int copied = 0;

  while (src.available())
  {
    String line = src.readStringUntil('\n');

    if (line.length() == 0)
      continue;

    rest += line;
    rest += '\n';

    copied++;

    Serial.print("Copy record #");
    Serial.println(copied);
  }

  src.close();

  Serial.println("Source ditutup.");

  // ============================================================
  // 5. NOW source sudah benar-benar ditutup
  //    Baru kita rewrite queue.
  // ============================================================

  Serial.println("Membuka queue untuk WRITE...");

  File dst = LittleFS.open(QUEUE_FILE, FILE_WRITE);

  if (!dst)
  {
    Serial.println("Gagal membuka queue untuk WRITE.");
    return false;
  }

  dst.print(rest);

  dst.close();

  Serial.println("Queue berhasil ditulis ulang.");

  Serial.print("Record tersisa: ");
  Serial.println(copied);

  Serial.println("Data offline berhasil terkirim dan dihapus dari queue.");

  return true;
}
// Entry point used by setup()/loop(): try to send immediately; if that
// fails for any reason (WiFi down, server unreachable, non-200 response),
// fall back to storing it in the offline queue instead of dropping it.
void httpPOST(char* tmp, char* hum, const struct tm &timeinfo){
  String json = buildJson(tmp, hum, timeinfo);
  if (!sendJson(json)) {
    queueReading(json);
  }
}

float hitungDewPoint(float suhu, float kelembapan){
    // Konstanta Magnus
    const float a = 17.27;
    const float b = 237.7;
    
    // Mencegah error matematika (infinity/NaN) jika kelembapan bernilai 0
    if (kelembapan <= 0.0) kelembapan = 0.01; 
    if (kelembapan > 100.0) kelembapan = 100.0;

    // Proses kalkulasi
    float alpha = log(kelembapan / 100.0) + ((a * suhu) / (b + suhu));
    float dewPoint = (b * alpha) / (a - alpha);
    
    return dewPoint;
}