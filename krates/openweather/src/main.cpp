
#include <Arduino.h>
#include <otaserver.h>
#include <kgfx.h>
#include <font_ArialBold.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>

OTAServer otaserver;
KGFX ui;

// APIKEY can be generated at https://openweathermap.org/current
const char *endpoint = "https://api.openweathermap.org/data/2.5/weather?appid={APIKEY}&zip={ZIPCODE}&units=imperial";
unsigned long lastTime = 0;
unsigned long timerDelay = 300000;
int requestCount = 0;

JsonDocument doc;

TFT_eSPI_ext tft = ui.tft();


void setup() { 
  Serial.begin(460800);
  Serial.println("Starting app");

  otaserver.connectWiFi(); // DO NOT EDIT.
  otaserver.run(); // DO NOT EDIT

  tft.begin();
  tft.fillScreen(TFT_BLACK);
}

void drawString(const char *s, int x, int y, ILI9341_t3_font_t font, uint16_t color) {
  tft.setTTFFont(font);
  tft.setTextColor(color,TFT_BLACK, true);
  tft.setCursor(x, y);
    Serial.print("Printing '"); Serial.print(s);
    Serial.print("' - width: ");Serial.print(tft.TTFtextWidth(s));
    Serial.print(" - height: ");Serial.print(tft.TTFLineSpace());Serial.println();
  
  tft.print(s);
  tft.print("    ");
  
}

void drawFloat(float f, int decimals, int x, int y, ILI9341_t3_font_t font, uint16_t color) {
  tft.setTTFFont(font);
  tft.setTextColor(color,TFT_BLACK, true);
  tft.setCursor(x, y);
  Serial.print("Printing '"); Serial.print(f,decimals);
  Serial.print("' - width: ");Serial.print(tft.TTFtextWidth("99"));
  Serial.print(" - height: ");Serial.print(tft.TTFtextHeight("9"));Serial.println();
  tft.print(f,decimals);tft.print("    ");
}


void renderSprite(const char *conditions, float temp, float minTemp, float maxTemp, int requestCount) {
  //tft.fillScreen(TFT_BLACK);
  drawString(conditions, 0, 0, Arial_24, TFT_WHITE);
  drawFloat(temp, 0, 120-31, 120-20, Arial_40_Bold, TFT_WHITE);
  drawFloat(minTemp, 0, 0, 240-18, Arial_18, TFT_WHITE);
  drawFloat(maxTemp, 0, 240-28, 240-18, Arial_18, TFT_WHITE);
  drawFloat(requestCount, 0, 0, 120-18, Arial_18, TFT_YELLOW);
}

void log(const char *conditions, float temp, float minTemp, float maxTemp) {
  Serial.print("Conditions: ");
  Serial.print(conditions);
  Serial.print(" - Temp: ");
  Serial.print(temp,0);
  Serial.print(" - Min: ");
  Serial.print(minTemp,0);
  Serial.print(" - Max: ");
  Serial.print(maxTemp,0);
  Serial.println();
}

void loop() {
  if((WiFi.status() == WL_CONNECTED)) {
    otaserver.handle(); // DO NOT EDIT

    
    if (((millis() - lastTime) > timerDelay) || lastTime == (unsigned long)(0)) {
      HTTPClient http;
      http.useHTTP10(true);
      http.begin(endpoint);
      int httpResponseCode = http.GET();
      requestCount++;

      char tempBuffer[5];
      char minTempBuffer[5];
      char maxTempBuffer[5];

      if (httpResponseCode>0) {
        deserializeJson(doc, http.getStream());
        http.end();

        float temp = doc["main"]["temp"];
        float minTemp = doc["main"]["temp_min"];
        float maxTemp = doc["main"]["temp_max"];
        const char* conditions = doc["weather"][0]["main"];

        renderSprite(conditions, temp, minTemp, maxTemp, requestCount);
        log(conditions, temp, minTemp, maxTemp);

      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      lastTime = millis();
    }
  }
  delay(1);
}

