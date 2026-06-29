#ifndef CLOCK_H
#define CLOCK_H

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "config.h"
#include "color_and_coordinates.h"
#include "timings.h"
#include "display.h"
#include "font_24x48_digits.h"
#include "font_36x72_digits.h"
#include "symbol.h"
#include "background.h"

extern Adafruit_ST7735 tft;

// ============================================================================
// СОСТОЯНИЕ ЧАСОВ
// ============================================================================
struct ClockState {
  bool isNtpSynced;
  bool isClockModeActive;
  unsigned long lastUpdateTime;
  unsigned long lastNtpSync;
  unsigned long lastInteractionTime;
  unsigned long pauseStartTime;
  int currentHour;
  int currentMinute;
  int currentDay;
  int currentMonth;
};

extern ClockState clockState;

// ============================================================================
// ПОИСК ГЛИФОВ В ШРИФТАХ ЧАСОВ
// ============================================================================
inline const uint8_t* getDigitGlyph24x48(uint16_t code) {
  int left = 0, right = GLYPH_COUNT_DIGITS_24x48 - 1;
  while (left <= right) {
    int mid = left + (right - left) / 2;
    uint16_t midCode = pgm_read_word(&glyphs_DIGITS_24x48[mid].code);
    if (midCode == code) {
      return (const uint8_t*)pgm_read_ptr(&glyphs_DIGITS_24x48[mid].data);
    }
    if (midCode < code) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }
  return nullptr;
}

inline const uint8_t* getDigitGlyph36x72(uint16_t code) {
  int left = 0, right = GLYPH_COUNT_DIGITS_36x72 - 1;
  while (left <= right) {
    int mid = left + (right - left) / 2;
    uint16_t midCode = pgm_read_word(&glyphs_DIGITS_36x72[mid].code);
    if (midCode == code) {
      return (const uint8_t*)pgm_read_ptr(&glyphs_DIGITS_36x72[mid].data);
    }
    if (midCode < code) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }
  return nullptr;
}

// ============================================================================
// ИНИЦИАЛИЗАЦИЯ NTP
// ============================================================================
inline void initNTP() {
  // Формируем строку часового пояса для configTzTime
  // В POSIX формате знак инвертирован: UTC+5 = "UTC-5"
  char tzString[16];
  if (NTP_TIMEZONE >= 0) {
    sprintf(tzString, "UTC-%d", NTP_TIMEZONE);
  } else {
    sprintf(tzString, "UTC+%d", -NTP_TIMEZONE);
  }
  
  configTzTime(tzString, NTP_SERVER);
  
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 5000)) {
    clockState.isNtpSynced = true;
    clockState.lastNtpSync = millis();
    Serial.print("NTP synchronized successfully with timezone: ");
    Serial.println(tzString);
  } else {
    clockState.isNtpSynced = false;
    Serial.println("NTP sync failed, using internal clock");
  }
}

// ============================================================================
// ОБНОВЛЕНИЕ ВРЕМЕНИ
// ============================================================================
inline void updateClockTime() {
  struct tm timeinfo;
  
  if (getLocalTime(&timeinfo, 100)) {
    clockState.currentHour = timeinfo.tm_hour;
    clockState.currentMinute = timeinfo.tm_min;
    clockState.currentDay = timeinfo.tm_mday;
    clockState.currentMonth = timeinfo.tm_mon + 1;
    
    if (!clockState.isNtpSynced) {
      clockState.isNtpSynced = true;
      Serial.println("NTP reconnected");
    }
  } else {
    // Если не удалось получить время, используем внутренние часы
    if (clockState.isNtpSynced) {
      clockState.isNtpSynced = false;
      Serial.println("NTP lost, switching to internal clock");
    }
  }
}

// ============================================================================
// ПРОВЕРКА АКТИВНОСТИ РЕЖИМА ЧАСОВ
// ============================================================================
inline void checkClockMode(int playbackStatus) {
  unsigned long now = millis();
  
  // Обновляем время последнего взаимодействия при нажатии кнопки
  // (вызывается из handleIRCommand)
  
  // Проверяем условия активации
  bool shouldActivate = false;
  
  if (playbackStatus == 0) {
    // Ничего не воспроизводится
    if (now - clockState.lastInteractionTime >= CLOCK_IDLE_TIMEOUT) {
      shouldActivate = true;
    }
  } else if (playbackStatus == 2) {
    // Пауза
    if (clockState.pauseStartTime == 0) {
      clockState.pauseStartTime = now;
    }
    if (now - clockState.pauseStartTime >= CLOCK_PAUSE_TIMEOUT) {
      shouldActivate = true;
    }
  } else {
    // Воспроизведение или перемотка
    clockState.pauseStartTime = 0;
    shouldActivate = false;
  }
  
  clockState.isClockModeActive = shouldActivate;
}

// ============================================================================
// ОТРИСОВКА ОДНОЙ ЦИФРЫ
// ============================================================================
inline void drawClockDigit(int x, int y, int digit, int fontWidth, int fontHeight, 
                          const uint8_t* (*getGlyph)(uint16_t), uint16_t color) {
  uint16_t code = 0x0030 + digit;
  const uint8_t* bitmap = getGlyph(code);
  if (bitmap) {
    tft.drawBitmap(x, y, bitmap, fontWidth, fontHeight, color);
  }
}

// ============================================================================
// ОТРИСОВКА ЧАСОВ И ДАТЫ
// ============================================================================
inline void drawClock() {
  // Определяем цвета в зависимости от источника времени
  uint16_t timeColor = clockState.isNtpSynced ? COLOR_CLOCK_TIME : COLOR_CLOCK_TIME_INTERNAL;
  uint16_t dateColor = clockState.isNtpSynced ? COLOR_CLOCK_DATE : COLOR_CLOCK_DATE_INTERNAL;
  
  // Очищаем область часов и даты
  clearArea(0, 0, TFT_WIDTH, TFT_HEIGHT, tft_background);
  
  // Рисуем дату (ДД/ММ) шрифтом 24x48
  int day1 = clockState.currentDay / 10;
  int day2 = clockState.currentDay % 10;
  int month1 = clockState.currentMonth / 10;
  int month2 = clockState.currentMonth % 10;
  
  drawClockDigit(CLOCK_DAY_X, CLOCK_DAY_Y, day1, 
                 FONT_WIDTH_DIGITS_24x48, FONT_HEIGHT_DIGITS_24x48, 
                 getDigitGlyph24x48, dateColor);
  
  drawClockDigit(CLOCK_DAY_X + FONT_WIDTH_DIGITS_24x48, CLOCK_DAY_Y, day2, 
                 FONT_WIDTH_DIGITS_24x48, FONT_HEIGHT_DIGITS_24x48, 
                 getDigitGlyph24x48, dateColor);
  
  // Рисуем слеш (24x48)
  tft.drawBitmap(CLOCK_SLASH_X, CLOCK_SLASH_Y, slashBitmap, 
                 SLASH_WIDTH, SLASH_HEIGHT, dateColor);
  
  drawClockDigit(CLOCK_MONTH_X, CLOCK_MONTH_Y, month1, 
                 FONT_WIDTH_DIGITS_24x48, FONT_HEIGHT_DIGITS_24x48, 
                 getDigitGlyph24x48, dateColor);
  
  drawClockDigit(CLOCK_MONTH_X + FONT_WIDTH_DIGITS_24x48, CLOCK_MONTH_Y, month2, 
                 FONT_WIDTH_DIGITS_24x48, FONT_HEIGHT_DIGITS_24x48, 
                 getDigitGlyph24x48, dateColor);
  
  // Рисуем время (ЧЧ:ММ) шрифтом 36x72
  int hour1 = clockState.currentHour / 10;
  int hour2 = clockState.currentHour % 10;
  int min1 = clockState.currentMinute / 10;
  int min2 = clockState.currentMinute % 10;
  
  drawClockDigit(CLOCK_HOURS_X, CLOCK_HOURS_Y, hour1, 
                 FONT_WIDTH_DIGITS_36x72, FONT_HEIGHT_DIGITS_36x72, 
                 getDigitGlyph36x72, timeColor);
  
  drawClockDigit(CLOCK_HOURS_X + FONT_WIDTH_DIGITS_36x72, CLOCK_HOURS_Y, hour2, 
                 FONT_WIDTH_DIGITS_36x72, FONT_HEIGHT_DIGITS_36x72, 
                 getDigitGlyph36x72, timeColor);
  
  // Рисуем двоеточие (10x72)
  tft.drawBitmap(CLOCK_COLON_X, CLOCK_COLON_Y, colonBitmap, 
                 COLON_WIDTH, COLON_HEIGHT, timeColor);
  
  drawClockDigit(CLOCK_MINUTES_X, CLOCK_MINUTES_Y, min1, 
                 FONT_WIDTH_DIGITS_36x72, FONT_HEIGHT_DIGITS_36x72, 
                 getDigitGlyph36x72, timeColor);
  
  drawClockDigit(CLOCK_MINUTES_X + FONT_WIDTH_DIGITS_36x72, CLOCK_MINUTES_Y, min2, 
                 FONT_WIDTH_DIGITS_36x72, FONT_HEIGHT_DIGITS_36x72, 
                 getDigitGlyph36x72, timeColor);
}
// ============================================================================
// ОБНОВЛЕНИЕ ЧАСОВ (вызывается из loop)
// ============================================================================
inline void updateClock(int playbackStatus) {
  unsigned long now = millis();
  
  // Проверяем режим часов
  checkClockMode(playbackStatus);
  
  // Если режим часов активен
  if (clockState.isClockModeActive) {
    // Обновляем время каждую минуту
    if (now - clockState.lastUpdateTime >= CLOCK_UPDATE_INTERVAL) {
      updateClockTime();
      drawClock();
      clockState.lastUpdateTime = now;
    }
    
    // Периодически синхронизируемся с NTP
    if (now - clockState.lastNtpSync >= NTP_SYNC_INTERVAL) {
      initNTP();
      clockState.lastNtpSync = now;
    }
  }
}

// ============================================================================
// СБРОС ТАЙМЕРА ВЗАИМОДЕЙСТВИЯ (вызывается при нажатии кнопки)
// ============================================================================
inline void resetInteractionTimer() {
  clockState.lastInteractionTime = millis();
  clockState.pauseStartTime = 0;
}

#endif // CLOCK_H
