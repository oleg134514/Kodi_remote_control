// config.h
#pragma once

// --- Wi-Fi Settings ---
const char* WIFI_SSID = "WIFI SSID";
const char* WIFI_PASS = "WIFI PASS";

// --- Kodi Settings ---
const char* KODI_IP = "192.168.0.100";
const int KODI_PORT = 8080;
const char* KODI_USER = "KODI USER";
const char* KODI_PASS = "KODI PASS";

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
const char* NTP_SERVER = "NTP SERVER IP";            // NTP сервер
const int NTP_TIMEZONE = 5;                         // Часовой пояс (например, 5 для UTC+5, -3 для UTC-3)
