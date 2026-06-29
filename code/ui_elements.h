#ifndef UI_ELEMENTS_H
#define UI_ELEMENTS_H

#include <Arduino.h>
#include "color_and_coordinates.h"
#include "display.h"
#include "background.h"
#include "bitmaps.h"
#include "ir_control.h"
#include "symbol.h"

extern Adafruit_ST7735 tft;

// ============================================================================
// СОСТОЯНИЕ ДИСПЛЕЯ
// ============================================================================
struct DisplayState {
  int playbackStatus;
  int volume;
  bool muted;
  int cpuLoad;
  float temperature;
  int currentSec;
  int totalSec;
  bool progressBarVisible;
  int previousSliderWidth;
};

extern DisplayState currentState;
extern DisplayState previousState;

// ============================================================================
// ОТРИСОВКА СТАТУСА ВОСПРОИЗВЕДЕНИЯ
// ============================================================================
inline void drawPlaybackStatus() {
  clearArea(STATUS_X, STATUS_Y, ICON_SIZE, ICON_SIZE, tft_background);
  
  if (currentState.playbackStatus == 1) {
    tft.drawRGBBitmap(STATUS_X, STATUS_Y, color_play, ICON_SIZE, ICON_SIZE);
  } else if (currentState.playbackStatus == 2) {
    tft.drawRGBBitmap(STATUS_X, STATUS_Y, color_pause, ICON_SIZE, ICON_SIZE);
  } else if (currentState.playbackStatus == 3) {
    tft.drawRGBBitmap(STATUS_X, STATUS_Y, color_forward, ICON_SIZE, ICON_SIZE);
  } else if (currentState.playbackStatus == 4) {
    tft.drawRGBBitmap(STATUS_X, STATUS_Y, color_rewind, ICON_SIZE, ICON_SIZE);
  }
}

// ============================================================================
// ОТРИСОВКА ГРОМКОСТИ
// ============================================================================
inline void drawVolumeDisplay() {
  if (currentState.muted) {
    tft.drawRGBBitmap(VOLUME_ICON_X, VOLUME_ICON_Y, color_mute, ICON_SIZE, ICON_SIZE);
  } else {
    tft.drawRGBBitmap(VOLUME_ICON_X, VOLUME_ICON_Y, color_volume, ICON_SIZE, ICON_SIZE);
  }
  
  uint16_t digitColor = currentState.muted ? COLOR_VOLUME_MUTED : COLOR_VOLUME_NORMAL;
  
  clearArea(VOLUME_DIGITS_X, VOLUME_DIGITS_Y, DIGIT_WIDTH * 3, DIGIT_HEIGHT, tft_background);
  
  int vol = currentState.volume;
  if (vol < 0) vol = 0;
  if (vol > 100) vol = 100;
  
  int startX = VOLUME_DIGITS_X;
  
  if (vol == 100) {
    uint16_t code1 = 0x0030 + 1;
    const uint8_t* bitmap1 = getDigitGlyphData(code1);
    if (bitmap1) {
      tft.drawBitmap(startX, VOLUME_DIGITS_Y, bitmap1, DIGIT_WIDTH, DIGIT_HEIGHT, digitColor);
    }
    
    uint16_t code2 = 0x0030 + 0;
    const uint8_t* bitmap2 = getDigitGlyphData(code2);
    if (bitmap2) {
      tft.drawBitmap(startX + DIGIT_WIDTH, VOLUME_DIGITS_Y, bitmap2, DIGIT_WIDTH, DIGIT_HEIGHT, digitColor);
    }
    
    uint16_t code3 = 0x0030 + 0;
    const uint8_t* bitmap3 = getDigitGlyphData(code3);
    if (bitmap3) {
      tft.drawBitmap(startX + DIGIT_WIDTH * 2, VOLUME_DIGITS_Y, bitmap3, DIGIT_WIDTH, DIGIT_HEIGHT, digitColor);
    }
  } else {
    int digit1 = vol / 10;
    int digit2 = vol % 10;
    
    uint16_t code1 = 0x0030 + digit1;
    const uint8_t* bitmap1 = getDigitGlyphData(code1);
    if (bitmap1) {
      tft.drawBitmap(startX, VOLUME_DIGITS_Y, bitmap1, DIGIT_WIDTH, DIGIT_HEIGHT, digitColor);
    }
    
    uint16_t code2 = 0x0030 + digit2;
    const uint8_t* bitmap2 = getDigitGlyphData(code2);
    if (bitmap2) {
      tft.drawBitmap(startX + DIGIT_WIDTH, VOLUME_DIGITS_Y, bitmap2, DIGIT_WIDTH, DIGIT_HEIGHT, digitColor);
    }
  }
}

// ============================================================================
// ОТРИСОВКА ИКОНКИ НАЖАТОЙ КНОПКИ
// ============================================================================
inline void drawPressedIcon(uint32_t code) {
  clearArea(PRESSED_ICON_X, PRESSED_ICON_Y, ICON_SIZE, ICON_SIZE, tft_background);
  
  for (int i = 0; i < irCommandsCount; i++) {
    if (irCommands[i].code == code) {
      tft.drawRGBBitmap(PRESSED_ICON_X, PRESSED_ICON_Y, irCommands[i].bitmap, ICON_SIZE, ICON_SIZE);
      break;
    }
  }
}

// ============================================================================
// ОТРИСОВКА CPU И ТЕМПЕРАТУРЫ
// ============================================================================
inline void drawCpuAndTemp() {
  int areaWidth = 100;
  int areaHeight = FONT_HEIGHT * 2 + 2;
  
  clearArea(CPU_X, CPU_Y, areaWidth, areaHeight, tft_background);
  
  // --- CPU ---
  char cpuLabel[8] = "CPU=";
  printText(cpuLabel, CPU_X, CPU_Y, COLOR_CPU_TEMP_LABEL);
  
  char cpuValue[8];
  if (currentState.cpuLoad < 0) {
    sprintf(cpuValue, "--");
  } else if (currentState.cpuLoad >= 100) {
    sprintf(cpuValue, "100");
  } else {
    sprintf(cpuValue, "%02d", currentState.cpuLoad);
  }
  
  uint16_t cpuValueColor = COLOR_CPU_TEMP_VALUE;
  if (currentState.cpuLoad >= CPU_CRITICAL_THRESHOLD && currentState.cpuLoad >= 0) {
    cpuValueColor = COLOR_CPU_CRITICAL;
  }
  
  printText(cpuValue, CPU_X + strlen(cpuLabel) * FONT_WIDTH, CPU_Y, cpuValueColor);
  printText("%", CPU_X + (strlen(cpuLabel) + strlen(cpuValue)) * FONT_WIDTH, CPU_Y, cpuValueColor);
  
  // --- Температура ---
  char tempLabel[4] = "T=";
  printText(tempLabel, TEMP_X, TEMP_Y, COLOR_CPU_TEMP_LABEL);
  
  char tempValue[8];
  if (currentState.temperature < 0) {
    sprintf(tempValue, "--.-");
  } else {
    sprintf(tempValue, "%04.1f", currentState.temperature);
  }
  
  uint16_t tempValueColor = COLOR_CPU_TEMP_VALUE;
  if (currentState.temperature >= TEMP_CRITICAL_THRESHOLD && currentState.temperature >= 0) {
    tempValueColor = COLOR_TEMP_CRITICAL;
  }
  
  printText(tempValue, TEMP_X + strlen(tempLabel) * FONT_WIDTH, TEMP_Y, tempValueColor);
  
  int degreeX = TEMP_X + (strlen(tempLabel) + strlen(tempValue) + 1) * FONT_WIDTH;
  tft.drawBitmap(degreeX, TEMP_Y, degreeSymbol, 7, 14, tempValueColor);
  printText("C", degreeX + 7, TEMP_Y, tempValueColor);
}

#endif // UI_ELEMENTS_H
