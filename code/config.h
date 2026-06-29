// config.h
#pragma once

// --- Wi-Fi Settings ---
const char* WIFI_SSID = "WL24";
const char* WIFI_PASS = "12H>*5x-g1?q8g0^I!R_J%F%CyCih_v-^3BYj=u5NV<O=b!8I^clj2WLS=Gjaa";

// --- Kodi Settings ---
const char* KODI_IP = "172.16.76.126";
const int KODI_PORT = 8080;
const char* KODI_USER = "kodi";
const char* KODI_PASS = "kodi";

// --- Pins ---
#define IR_RECEIVER_PIN 4
#define TFT_RES 1
#define TFT_DC 3
#define TFT_SCK 6
#define TFT_SDA 7
#define TFT_CS 10

// ============================================================================
// NTP НАСТРОЙКИ
// ============================================================================
const char* NTP_SERVER = "172.16.76.59";            // NTP сервер
const int NTP_TIMEZONE = 5;                         // Часовой пояс (например, 5 для UTC+5, -3 для UTC-3)
