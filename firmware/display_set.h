#define SRV_OK 0
#define SRV_NOK 1
#define SRV_DOWN 2



unsigned char tempX=6, tempY=108;
unsigned char rhX=6, rhY=170; 
unsigned char dpX=6, dpY=233;
uint16_t myColor = tft.color565(11, 70, 157);

void waterdrop(unsigned char dropX, unsigned char dropY, unsigned char dropR)
{
 // unsigned char dropX=30, dropY=180, dropR=5, drip=20;
 unsigned char drip = 1.5 * dropR;
  tft.fillCircle(dropX, dropY+(2*dropR), dropR, TFT_WHITE);
  tft.fillTriangle(
      dropX, dropY, // peak
      dropX - dropR+1, dropY + drip, // bottom left
      dropX + dropR-1, dropY + drip, // bottom right
      TFT_WHITE);
}

void mercury()
{
  tft.fillCircle(33, 145, 10, TFT_WHITE);
  tft.setTextDatum(TL_DATUM);
  tft.fillRoundRect(27, 116, 12, 40, 6, TFT_WHITE);
  tft.fillCircle(33, 145, 7, myColor);
  tft.fillRect(30, 127, 6, 25, myColor);
}
int pngDraw(PNGDRAW *pDraw) 
{
    uint16_t lineBuffer[MAX_IMAGE_WIDTH];

    png.getLineAsRGB565(
        pDraw,
        lineBuffer,
        PNG_RGB565_BIG_ENDIAN,
        0xffffffff);

    tft.pushImage(
        xpos,
        ypos + pDraw->y,
        pDraw->iWidth,
        1,
        lineBuffer);

    return 1;   // lanjut decode
}
void logo()
{
 
    int16_t rc = png.openFLASH((uint8_t *)logopng, sizeof(logopng), pngDraw);
  if (rc == PNG_SUCCESS) {
    Serial.println("Successfully opened png file");
    Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
    tft.startWrite();
    uint32_t dt = millis();
    rc = png.decode(NULL, 0);
    Serial.print(millis() - dt); Serial.println("ms");
    tft.endWrite();
    tft.setFreeFont(FF17);
    tft.setTextSize(1);
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.drawString("UNIVERSITAS TRISAKTI", 120, 88, 1);
  }
}

void tftBoot()
{
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK,  TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextDatum(TC_DATUM);
  tft.setFreeFont(FF21);
  tft.drawString("Initializing...", 120, 30, 1);
  tft.setFreeFont(FF21);
  tft.drawString("Hold Boot button to Reset", 120, 60, 1);  
}

void tftWifiProvis()
{
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK,  TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextDatum(TC_DATUM);
  tft.setFreeFont(FF21);
  tft.drawString("Connect to WiFi SSID:", 120, 110, 1);
  tft.setFreeFont(FF22);
  tft.drawString("Minilog-Pro32", 120, 140, 1);
  tft.setFreeFont(FF21);
  tft.drawString("Passphrase:", 120, 180, 1);
  tft.setFreeFont(FF22);
  tft.drawString("minilog32", 120, 210, 1);
  tft.setFreeFont(FF21);
  tft.drawString("open on your browser", 120, 240, 1);
  tft.setFreeFont(FF22);
  tft.drawString("192.168.1.4", 120, 260, 1);
  logo();
  
}
void saveParamCallback() {

  Serial.println("Saving parameters");

  Sen_mode = wm.server->arg("mode");

  Serial.print("Mode selected: ");
  Serial.println(Sen_mode);
}
void tftWifiConnected()
{
  tft.fillScreen(TFT_WHITE);
  logo();
  tft.setTextColor(TFT_BLACK,  TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextDatum(TC_DATUM);
  tft.setFreeFont(FF22);
  tft.drawString("WiFi Connected", 120, 110, 1);

  //Update Parameter string from WM
  // APIhost = API_host.getValue();
  // NTPserver = NTP_server.getValue();
  // ID_Node = ID_node.getValue();
  // //Sen_mode = wm.server->arg("mode");//Sensor_mode.getValue();
  // log_user = LogUser.getValue();
  // Log_interval = LogInt.getValue();

  //Record parameter string to Flash (Non Volatile)
  // if (APIhost.length() > 0) {
  //   preferences.putString("APIhost", APIhost);
  //   Serial.print("Saved APIhost: ");
  //   Serial.println(APIhost);
  // } else {
  //   // Load previously stored Room ID if user left it empty
  //   String saved = preferences.getString("APIhost", "");
  //   if (saved.length() > 0) {
  //     Serial.print("Loaded stored API Host: ");
  //     Serial.println(saved);
  //     APIhost = saved;
  //   } else {
  //     Serial.println("No APIhost set yet.");
  //   }
  // }

  // if (NTPserver.length() > 0) {
  //   preferences.putString("NTPserver", NTPserver);
  //   Serial.print("NTP Server Updated: ");
  //   Serial.println(NTPserver);
  // } else {
  //   // Load previously stored Room ID if user left it empty
  //   String saved = preferences.getString("NTPserver", "");
  //   if (saved.length() > 0) {
  //     Serial.print("Loaded stored NTP Server: ");
  //     Serial.println(saved);
  //     NTPserver = saved;
  //   } else {
  //     Serial.println("No NTP Server set yet.");
  //   }
  // }

  
 
  // if (ID_Node.length() > 0) {
  //   preferences.putString("ID_Node", ID_Node);
  //   Serial.print("ID Suhu Updated: ");
  //   Serial.println(ID_Node);
  // } else {
  //   // Load previously stored Room ID if user left it empty
  //   String saved = preferences.getString("ID_Node", "");
  //   if (saved.length() > 0) {
  //     Serial.print("Loaded stored ID Suhu: ");
  //     Serial.println(saved);
  //     ID_Node = saved;
  //   } else {
  //     Serial.println("No ID Suhu set yet.");
  //   }
  // }
  // tft.setTextDatum(TR_DATUM);
  // tft.setFreeFont(FF21);
  // //tft.drawString("ID Node :", 120, 70, 1);
  // tft.setTextDatum(TL_DATUM);
  // tft.setFreeFont(FF21);
  // //tft.drawString(ID_Node, 130, 70, 1);


  // if (Sen_mode.length() > 0) {
  //   preferences.putString("Sen_mode", Sen_mode);
  //   Serial.print("ID Humid Updated: ");
  //   Serial.println(Sen_mode);
  // } else {
  //   // Load previously stored Room ID if user left it empty
  //   String saved = preferences.getString("Sen_mode", "");
  //   if (saved.length() > 0) {
  //     Serial.print("Loaded stored ID Humid: ");
  //     Serial.println(saved);
  //     Sen_mode = saved;
  //   } else {
  //     Serial.println("No ID Humid set yet.");
  //   }
  // }

  // tft.setTextDatum(TR_DATUM);
  // tft.setFreeFont(FF21);
  // //tft.drawString("Sens Mode :", 120, 100, 1);
  // tft.setTextDatum(TL_DATUM);
  // tft.setFreeFont(FF21);
  // //tft.drawString(Sen_mode, 130, 100, 1);
  

  // if (log_user.length() > 0) {
  //   preferences.putString("log_user", log_user);
  //   Serial.print("Log User Updated: ");
  //   Serial.println(log_user);
  // } else {
  //   // Load previously stored Room ID if user left it empty
  //   String saved = preferences.getString("log_user", "");
  //   if (saved.length() > 0) {
  //     Serial.print("Loaded stored Log User: ");
  //     Serial.println(saved);
  //     log_user = saved;
  //   } else {
  //     Serial.println("No Log User set yet.");
  //   }
  // }

  // tft.setTextDatum(TR_DATUM);
  // tft.setFreeFont(FF21);
  // //tft.drawString("Log User :", 120, 130, 1);
  // tft.setTextDatum(TL_DATUM);
  // tft.setFreeFont(FF21);
  // //tft.drawString(log_user, 130, 130, 1);

  // if (Log_interval.length() > 0) {
  //   preferences.putString("Log_interval", Log_interval);
  //   Serial.print("Log Interval Updated: ");
  //   Serial.println(Log_interval);
  // } else {
  //   // Load previously stored Room ID if user left it empty
  //   String saved = preferences.getString("Log_interval", "");
  //   if (saved.length() > 0) {
  //     Serial.print("Loaded stored Log Interval: ");
  //     Serial.println(saved);
  //     Log_interval = saved;
  //   } else {
  //     Serial.println("No Log interval set yet.");
  //   }
  // }

  // tft.setTextDatum(TR_DATUM);
  // tft.setFreeFont(FF21);
  // //tft.drawString("Log Interval :", 120, 160, 1);
  // tft.setTextDatum(TL_DATUM);
  // tft.setFreeFont(FF21);
  // //tft.drawString(Log_interval, 130, 160, 1);

  // preferences.end();
}

void tftinitSHT()
{
  tft.fillScreen(TFT_WHITE);
  logo();
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextDatum(TC_DATUM);
  tft.setFreeFont(FF22);
  tft.drawString("Initializing Sensor", 120, 110, 1);
}
void tftinitRTC(const char* ntpchar)
{
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(1);
  tft.setTextDatum(TC_DATUM);
  tft.setFreeFont(FF22);
  tft.drawString("NTP Sync", 120, 150, 1);
  tft.setFreeFont(FF17);
  tft.drawString(ntpchar, 120, 175, 1);

}

/*void timeNdate()
{
  struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
    }
    
  char timeStr[6];
  strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);

  // Format date as "dd/mm/yyyy"
  char dateStr[11];
  strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &timeinfo);

  tft.setTextSize(1);
  tft.setTextDatum(TC_DATUM);
  tft.setFreeFont(FF23);
  tft.setTextColor(myColor, TFT_WHITE);
  tft.drawString(timeStr, 120, 30, 1);
  tft.setTextDatum(TC_DATUM);
  tft.setFreeFont(FF21);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString(dateStr, 120, 62, 1);
  tft.setTextDatum(TL_DATUM);
  //tft.drawString(roomID, 5, 30, 1);
}*/

void timeNdate(const struct tm &timeinfo)
{
  char timeStr[6];
  strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);

  char dateStr[11];
  strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &timeinfo);

  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);
  tft.setFreeFont(FF21);
  tft.setTextColor(myColor, TFT_WHITE);
  tft.fillRect(9, 294, 100, 20, TFT_WHITE);
  tft.drawString(timeStr, 10, 295, 1);

  tft.setFreeFont(FF21);
  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  
  tft.fillRect(120, 294, 120, 20, TFT_WHITE);
  tft.drawString(dateStr, 228, 295, 1);

  tft.setTextDatum(TL_DATUM);
}

unsigned char getRSSI()
{
  int rssi = WiFi.RSSI();
  unsigned char bars;

  if (rssi > -55) {
    bars = 4;  // Excellent
  } else if (rssi > -67) {
    bars = 3;  // Good
  } else if (rssi > -70) {
    bars = 2;  // Fair
  } else if (rssi > -80) {
    bars = 1;  // Weak
  } else {
    bars = 0;  // Unusable
  }
  return(bars);
}

void WiFiStat()
{
  unsigned char i = 0, bar=0;
  bar = getRSSI();
  for(i=1;i<5;i++)
  {
    if(i>bar)
    {
    tft.fillRect((i*5)+214, 18 - (i+1)*3,  3, (i+1)*3, TFT_WHITE);
    }
    else
    {
    tft.fillRect((i*5)+214, 18 - (i+1)*3,  3, (i+1)*3, TFT_BLACK);
    }
  }
}

void tftSSID()
{
  tft.setTextSize(1);
  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString(WiFi.SSID(), 215, 3, 2);
}

void tftSet(bool setv)
{
  if(setv){
    tft.setTextSize(1);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.drawString("Set", 20, 3, 2);
  }
  else
  {
    tft.fillRect(20,3,30, 15, TFT_WHITE);
  }
}

void serverStat(char stts)
{
  if(stts==SRV_OK)tft.fillCircle(10, 10, 5, TFT_GREEN); 
  if(stts==SRV_NOK)tft.fillCircle(10, 10, 5, TFT_YELLOW); 
  if(stts==SRV_DOWN)tft.fillCircle(10, 10, 5, TFT_RED); 
  
}

void battery()
{
  char battX = 178;
  tft.drawRoundRect(battX, 4, 24, 13, 3, tft.color565(0, 0, 0));
  tft.fillRect(battX + 2, 6,  11, 9, tft.color565(0, 255, 0));
  //tft.setFreeFont(FF21);
  tft.setTextSize(1);
  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("100%", 237, 3, 2);
}

void Drawspace()
{
  //Draw Temp Display Background
  tft.fillRoundRect(tempX, tempY, 230, 55, 10, myColor);
  //Draw RH Display Background
  tft.fillRoundRect(rhX, rhY, 230, 55, 10, myColor);
  //Draw DP Display Background
  tft.fillRoundRect(dpX, dpY, 230, 55, 10, myColor);
 
}

void Temp(char dt[6])
{
  tft.setFreeFont(FF20);
  tft.setTextSize(1);
  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(TFT_WHITE, myColor);
  tft.fillRect(53, tempY+2, 40, 50, myColor);
  tft.drawString(dt, tempX+158, tempY+9, 1);
  tft.setTextDatum(TL_DATUM);
  tft.setFreeFont(FF22);
  tft.drawString("C", tempX+180, tempY+14, 1);
  tft.fillCircle(tempX+170, tempY+15, 7, TFT_WHITE);
  tft.fillCircle(tempX+170, tempY+15, 4, myColor);
  mercury();
}

void RH(char dt[6])
{
  tft.setFreeFont(FF20);
  tft.setTextSize(1);
  tft.setTextDatum(TR_DATUM);
  //tft.fillRoundRect(rhX, rhY, 230, 55, 10, myColor);
  tft.setTextColor(TFT_WHITE, myColor);
  tft.fillRect(53, rhY+2, 40, 50, myColor);
  tft.drawString(dt, rhX+158, rhY+9, 1);
  tft.setTextSize(1);
  tft.setTextDatum(TL_DATUM);
  tft.setFreeFont(FF21);
  tft.drawString("%RH", rhX+163, rhY+9, 1);
  waterdrop(20, 180, 6);
  waterdrop(33, 191, 8);
  waterdrop(47, 180, 5);
}

void dew(char dt[6])
{
  tft.setFreeFont(FF20);
  tft.setTextSize(1);
  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(TFT_WHITE, myColor);
  tft.fillRect(53, dpY+2, 40, 50, myColor);
  //tft.fillRoundRect(dpX, dpY, 230, 55, 10, myColor);
  tft.drawString(dt, dpX+158, dpY+9, 1);
  tft.fillCircle(dpX+170, dpY+15, 7, TFT_WHITE);
  tft.fillCircle(dpX+170, dpY+15, 4, myColor);
  tft.setFreeFont(FF22);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("DP", dpX+12, dpY+14, 1);
  tft.drawString("C", dpX+180, dpY+14, 1);
}
