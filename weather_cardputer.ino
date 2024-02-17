#include <M5Cardputer.h>

#include <M5GFX.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "";
const char* password = "";

const char* openWeatherMapApiKey = "";
const char* city = "";

int page = 1;

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextFont(&fonts::FreeSans18pt7b);
  M5Cardputer.Display.setTextColor(WHITE);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    getWeatherData();
}

void loop() {
  M5Cardputer.update();

  static unsigned long lastTaskTime = 0;
  const unsigned long interval = 60000;

  if (millis() - lastTaskTime >= interval) {
    getWeatherData();
    lastTaskTime = millis();
  }

  if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isKeyPressed(',')) {
          M5Cardputer.Speaker.tone(3000, 40);
          pagePrev();
  } else if (M5Cardputer.Keyboard.isKeyPressed('/')) {
    M5Cardputer.Speaker.tone(3000, 40);
    pageNext();
  }
}

  if (M5Cardputer.BtnA.wasPressed()) {
      M5Cardputer.Speaker.tone(3450, 40);
      getWeatherData();
  }
}

void pageNext() {
  if (page == 2) {
    page = 1;
    getWeatherData();
  } else {
    page++;
    getWeatherData();
  }
}

void pagePrev() {
  if (page == 1) {
    page = 2;
    getWeatherData();
  } else {
    page--;
    getWeatherData();
  }
}

void showPage() {
    switch (page) {
      case 1:
        M5Cardputer.Display.setTextSize(0.75);
        M5Cardputer.Display.drawString("= -", 98, 108);
        M5Cardputer.Display.setTextSize(1);
        break;
      case 2:
        M5Cardputer.Display.setTextSize(0.75);
        M5Cardputer.Display.drawString("- =", 98, 108);
        M5Cardputer.Display.setTextSize(1);
        break;
  }
}

void getWeatherData() {
  M5Cardputer.update();
  HTTPClient http;
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "&appid=" + String(openWeatherMapApiKey) + "&units=metric";
  http.begin(url);

  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      DynamicJsonDocument jsonBuffer(1024);
      deserializeJson(jsonBuffer, http.getString());

      float temperature = jsonBuffer["main"]["temp"];
      float pressure = jsonBuffer["main"]["pressure"];
      String description = jsonBuffer["weather"][0]["description"];
      
      switch (page) {
        case 1:
          M5Cardputer.Display.clear();
          M5Cardputer.Display.fillScreen(BLACK);
          M5Cardputer.Display.setTextSize(1.5);
          M5Cardputer.Display.drawString(String(temperature) + " C", 45, 45);
          showPage();
        break;
        case 2:
          M5Cardputer.Display.clear();
          M5Cardputer.Display.fillScreen(BLACK);
          M5Cardputer.Display.setTextSize(1);
          M5Cardputer.Display.drawString(String(temperature) + " C", 5, 5);
          M5Cardputer.Display.drawString(String(pressure) + " hPa", 5, 45);
          M5Cardputer.Display.setTextSize(0.75);
          M5Cardputer.Display.drawString(String(description), 5, 85);
          showPage();
        break;
      }

    }
  } else {
    M5Cardputer.Display.drawString("Failed to get information from OpenWeatherMap API", 5, 5);
  }

  http.end();
}
