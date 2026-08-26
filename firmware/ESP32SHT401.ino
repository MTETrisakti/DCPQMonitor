
// add pngdec by larry bank in the lib manager
#include <PNGdec.h>
#include "logo.h" // Image is stored here in an 8-bit array
#include <FS.h>
#include "math.h"
using FS = fs::FS;
PNG png; // PNG decoder instance

#define MAX_IMAGE_WIDTH 240 // Adjust for your images
#define DevID 2
#define CorrT 1
#define CorrRH 1
#define LCDBL 27


// Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
//add TFT_eSPI by bodmer in the library manager
//check the TFT_eSPI library folder, replace the original User_Setup.h with the one comes with this .ino 
#include <TFT_eSPI.h>     // Hardware-specific library
#include "FreeFont.h"     
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "Adafruit_SHT4x.h"
#include <Time.h>
#include <WiFi.h>
#include <HTTPClient.h>
//add arduinojson by benoit in the library manager
#include <ArduinoJson.h>
//add WiFiManager library in the library manager
#include <WiFiManager.h>
#include <Preferences.h>
#include <LittleFS.h>


// Custom parameter for Room ID
// WiFiManagerParameter API_host("APIhost", "API Host", "", 255);
// WiFiManagerParameter NTP_server("NTPserver", "NTP Server", "", 255);
// WiFiManagerParameter ID_node("ID_node", "ID Node", "", 8);
// WiFiManagerParameter Sensor_mode(
// "<label for='mode'>Sensor Mode</label>"
// "<select name='mode'>"
// "<option value='SHT40'>SHT40</option>"
// "<option value='Optilux'>Optilux</option>"
// "<option value='DP-800'>DP-800</option>"
// "<option value='485PRO'>485PRO</option>"
// "</select>"
// );
// WiFiManagerParameter LogInt("log_interval", "Log Interval (menit)", "", 5);
// WiFiManagerParameter LogUser("log_user", "User", "", 32);

#define QUEUE_FILE       "/queue.jsonl"
#define QUEUE_MAX_BYTES  (200UL * 1024UL) // cap so a long outage can't fill the flash
#define FORMAT_LITTLEFS_ON_BOOT true

Preferences preferences;

bool wmReset;

unsigned long delayTime;
Adafruit_SHT4x sht40;

TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
int16_t xpos=80;
int16_t ypos=10;
String APIhost, NTPserver, ID_Node, Sen_mode, log_user, Log_interval;
// SECURITY: replace with YOUR OWN Apps Script Web App deployment URL
// (Deploy > New deployment > Web app in the Apps Script editor for cloud/code.gs).
// Do not reuse the URL from another deployment -- it is bound to that
// deployment's spreadsheet and secret token.
const char* serverName = "https://script.google.com/macros/s/YOUR_DEPLOYMENT_ID/exec";



const long  gmtOffset_sec = 7 * 3600;   // GMT+7 (WIB)
const int   daylightOffset_sec = 0;     // no daylight saving in Indonesia
sensors_event_t s40humidity, s40temp;
WiFiManager wm;

#include "display_set.h"
#include "fn.h"
//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  //init serial comm
  Serial.begin(115200);
  delay(1000);
  //init preferences unit
  preferences.begin("config", false);
  //init wifi manager
 

  tft.begin();
  delay(1000);
  tftBoot();
  delay(2000);
  pinMode(35, INPUT);
  if(digitalRead(35) == LOW){
    Serial.println("Reset Setup");
    wm.resetSettings();
  }
  
    pinMode(LCDBL, OUTPUT);
  digitalWrite(LCDBL, HIGH);
  // Add our custom parameter before autoConnect()
  //wm.addParameter(&API_host);
  //wm.addParameter(&NTP_server);
  //wm.addParameter(&ID_node);
  //wm.addParameter(&Sensor_mode);
  //wm.addParameter(&LogInt);
  //wm.addParameter(&LogUser);
  //wm.setSaveParamsCallback(saveParamCallback);
  

  //init tft functions
  tftWifiProvis();
  // Try to connect to saved WiFi, else start config portal
  if (!wm.autoConnect("Minilog-Pro32", "minilog32")) {
    Serial.println("Failed to connect and hit timeout");
    ESP.restart();
  }
  tftWifiConnected();
  delay(2000);

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed — offline queue disabled!");
  } else {
    Serial.println("LittleFS mounted OK.");
  }

  #if FORMAT_LITTLEFS_ON_BOOT

    Serial.println("==============================");
    Serial.println("FORMATTING LITTLEFS...");
    Serial.println("==============================");

    if (LittleFS.format())
    {
        Serial.println("LittleFS FORMAT SUCCESS!");
    }
    else
    {
        Serial.println("LittleFS FORMAT FAILED!");
    }

  #endif

  tftinitSHT();
  initSHT();
  delay(2000);
  const char* NTPsrv = "216.239.35.12";//NTPserver.c_str();
  tftinitRTC(NTPsrv);
  configTime(gmtOffset_sec, daylightOffset_sec, NTPsrv);
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) 
  {
    Serial.println("NTP Sync Fail");
    delay(1000);
  }
    Serial.println("NTP Sync Success");
  tft.fillScreen(TFT_WHITE);
  logo();
  timeNdate(timeinfo);
  tftSSID();
  Drawspace();
  if (sht40.getEvent(&s40humidity, &s40temp)) {
     char shtTemp[6], shtRH[6];
    dtostrf(s40temp.temperature, 4, 1, shtTemp);
    dtostrf(s40humidity.relative_humidity * 0.86, 4, 1, shtRH);  
    Temp(shtTemp);
    RH(shtRH);
    dew(shtTemp);
    httpPOST(shtTemp, shtRH, timeinfo);
  } else {
    Serial.println("Failed to read from SHT40!");
  }
  
}

//====================================================================================
//                                    Loop
//====================================================================================
unsigned long lastSecond = 0;
const unsigned long interval = 1000;

int lastPrintedMinute = -1;
int lastPostSecond = -1;

void loop() {

  unsigned long now = millis();

  // Run once every 1 second
  if (now - lastSecond >= interval) {
    lastSecond += interval;   // lebih stabil daripada lastSecond = now

    WiFiStat();

    // -------------------------------------------------------------------------
    // Read temperature & humidity
    // -------------------------------------------------------------------------
    if (sht40.getEvent(&s40humidity, &s40temp)) {

      char shtTemp[6], shtRH[6], Dep[6];

      float Te, Re, De;

      Te = s40temp.temperature * CorrT;
      Re = s40humidity.relative_humidity * CorrRH;
      De = hitungDewPoint(Te, Re);

      dtostrf(Te, 4, 1, shtTemp);
      Temp(shtTemp);

      dtostrf(Re, 4, 1, shtRH);
      RH(shtRH);

      dtostrf(De, 4, 1, Dep);
      dew(Dep);


      // -----------------------------------------------------------------------
      // Get RTC / NTP time
      // -----------------------------------------------------------------------
      struct tm timeinfo;

      if (getLocalTime(&timeinfo)) {

        // ================================================================
        // HTTP POST setiap detik 00 dan 30
        // ================================================================
        if ((timeinfo.tm_sec == 0 || timeinfo.tm_sec == 30) &&
            timeinfo.tm_sec != lastPostSecond) {

          httpPOST(shtTemp, shtRH, timeinfo);

          lastPostSecond = timeinfo.tm_sec;
        }
        else
        {
          if (WiFi.status() == WL_CONNECTED) {
          flushOneQueuedReading();
          }
        }


        // ================================================================
        // Update display ketika menit berubah
        // ================================================================
        if (timeinfo.tm_min != lastPrintedMinute) {

          lastPrintedMinute = timeinfo.tm_min;

          timeNdate(timeinfo);
        }

      }

    } else {

      Serial.println("Failed to read from SHT40!");

    }
  }

  // other non-blocking background tasks can run here
}




//=========================================v==========================================
//                                      pngDraw
//====================================================================================
// This next function will be called during decoding of the png file to
// render each image line to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
// Callback function to draw pixels to the display

