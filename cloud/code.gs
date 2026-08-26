/**
 * Google Apps Script backend for the DCPQ Monitor.
 *
 * Deploy this script bound to a Google Sheet (Extensions > Apps Script),
 * then Deploy > New deployment > Web app (execute as "Me", access "Anyone")
 * to obtain the exec URL used as `serverName` in firmware/ESP32SHT401.ino.
 *
 * doPost(): receives one raw {temperature, humidity, deviceID, timestamp,
 * token} JSON reading over HTTPS from the ESP32 device and appends it as an
 * immutable row (timestamp, temperature, humidity, device_id) to the active
 * sheet. Psychrometric derivation (dew point, wet-bulb, enthalpy, etc.) is
 * intentionally NOT computed here -- it is computed on demand by a
 * corresponding doGet() dashboard handler (not included in this snippet)
 * so the stored raw schema stays minimal and auditable. See Section 4.2 of
 * the manuscript for the full data pipeline description.
 */

function doPost(e) {
  var lock = LockService.getScriptLock();
  // Menunggu antrean jika ada data masuk bersamaan agar tidak bentrok
  lock.tryLock(15000);

  // SECURITY: set your own secret here and keep it out of version control
  // in your real deployment (e.g. via Script Properties instead of a
  // hardcoded literal). Must match TOKEN_SECRET in firmware/fn.h.
  var secret = "YOUR_SECRET_TOKEN_HERE";

  try {
    // 1) VALIDASI DATA MASUK
    if (!e.postData || !e.postData.contents) {
      return createJsonResponse({ status: "no_data" });
    }

    var data = JSON.parse(e.postData.contents);

    // 2) VALIDASI TOKEN
    if (data.token !== secret) {
      return createJsonResponse({ status: "unauthorized" });
    }

    // 3) VALIDASI DEVICE ID
    var id = parseInt(data.deviceID);
    if (isNaN(id) || id < 1) {
      return createJsonResponse({ status: "invalid_id" });
    }

    // 4) TERIMA TIMESTAMP APA ADANYA (TRANSPARAN)
    // Mengambil waktu langsung dari payload ESP32 tanpa pembulatan
    var incomingTime = new Date(data.timestamp);

    if (isNaN(incomingTime.getTime())) {
      return createJsonResponse({ status: "invalid_timestamp" });
    }

    // 5) AKSES SHEET
    var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();

    // 6) TULIS SEBAGAI BARIS BARU (APPEND)
    // Format Long Data: Hanya 4 kolom (A, B, C, D)
    var newRowData = [
      incomingTime,      // Kolom A: Waktu asli dari ESP32
      data.temperature,  // Kolom B: Suhu
      data.humidity,     // Kolom C: Kelembapan
      id                 // Kolom D: ID Device
    ];

    // Langsung tambahkan ke baris paling bawah
    sheet.appendRow(newRowData);

    return createJsonResponse({ status: "success" });

  } catch (err) {
    return createJsonResponse({ status: "error", message: err.toString() });
  } finally {
    // Lepas kunci agar data berikutnya bisa masuk
    lock.releaseLock();
  }
}

// Fungsi pembantu response JSON
function createJsonResponse(obj) {
  return ContentService.createTextOutput(JSON.stringify(obj))
    .setMimeType(ContentService.MimeType.JSON);
}
