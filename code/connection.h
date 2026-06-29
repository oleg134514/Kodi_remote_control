#ifndef CONNECTION_H
#define CONNECTION_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"
#include "color_and_coordinates.h"
#include "timings.h"
#include "display.h"
#include "background.h"
#include "bitmaps.h"

extern bool systemReady;

// ============================================================================
// ПОСЛЕДОВАТЕЛЬНАЯ ИНИЦИАЛИЗАЦИЯ
// ============================================================================
inline bool performSequencedInitialization() {
  tft.fillScreen(ST77XX_BLACK);
  
  bool passWiFi = false, passServer = false, passAuth = false;
  
  // --- Проверка WiFi ---
  printText("Подключение к", 0, 10, COLOR_STATUS_TEXT);
  printText("WI-FI", 0, 25, COLOR_STATUS_TEXT);
  printText("..........", 40, 25, COLOR_STATUS_TEXT);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < WIFI_CONNECT_ATTEMPTS) {
    delay(WIFI_CONNECT_TIMEOUT);
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    printText("OK", 120, 25, COLOR_STATUS_OK);
    delay(STATUS_DISPLAY_DELAY_OK);
    passWiFi = true;
  } else {
    printText("error", 120, 25, COLOR_STATUS_ERROR);
    delay(STATUS_DISPLAY_DELAY_ERROR);
  }
  
  // --- Проверка сервера ---
  printText("Подключение к", 0, 45, COLOR_STATUS_TEXT);
  printText("серверу", 0, 65, COLOR_STATUS_TEXT);
  printText("........", 56, 65, COLOR_STATUS_TEXT);
  
  if (passWiFi) {
    WiFiClient testClient;
    testClient.setTimeout(SERVER_CONNECT_TIMEOUT);
    if (testClient.connect(KODI_IP, KODI_PORT)) {
      printText("OK", 120, 65, COLOR_STATUS_OK);
      delay(STATUS_DISPLAY_DELAY_OK);
      passServer = true;
      testClient.stop();
    } else {
      printText("error", 120, 65, COLOR_STATUS_ERROR);
      delay(STATUS_DISPLAY_DELAY_ERROR);
    }
  } else {
    printText("error", 120, 65, COLOR_STATUS_ERROR);
    delay(STATUS_DISPLAY_DELAY_ERROR);
  }
  
  // --- Проверка авторизации ---
  printText("Авторизация", 0, 85, COLOR_STATUS_TEXT);
  printText("....", 88, 85, COLOR_STATUS_TEXT);
  
  if (passServer) {
    HTTPClient httpAuth;
    httpAuth.begin(String("http://") + KODI_IP + ":" + KODI_PORT + "/jsonrpc");
    httpAuth.setAuthorization(KODI_USER, KODI_PASS);
    httpAuth.addHeader("Content-Type", "application/json");
    httpAuth.setTimeout(AUTH_TIMEOUT);
    
    int httpCode = httpAuth.POST(R"({"jsonrpc":"2.0","method":"JSONRPC.Ping","id":1})");
    
    if (httpCode == HTTP_CODE_OK) {
      String res = httpAuth.getString();
      if (res.indexOf("pong") != -1) {
        printText("OK", 120, 85, COLOR_STATUS_OK);
        delay(STATUS_DISPLAY_DELAY_OK);
        passAuth = true;
      } else {
        printText("error", 120, 85, COLOR_STATUS_ERROR);
        delay(STATUS_DISPLAY_DELAY_ERROR);
      }
    } else {
      printText("error", 120, 85, COLOR_STATUS_ERROR);
      delay(STATUS_DISPLAY_DELAY_ERROR);
    }
    httpAuth.end();
  } else {
    printText("error", 120, 65, COLOR_STATUS_ERROR);
    delay(STATUS_DISPLAY_DELAY_ERROR);
  }
  
  delay(FINAL_STATUS_DELAY);
  
  if (passWiFi && passServer && passAuth) {
    tft.drawRGBBitmap(0, 0, tft_background, TFT_WIDTH, TFT_HEIGHT);
    return true;
  } else {
    tft.fillScreen(ST77XX_BLACK);
    return false;
  }
}

#endif // CONNECTION_H
