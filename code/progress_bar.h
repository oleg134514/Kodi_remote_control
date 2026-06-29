#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <Arduino.h>
#include "color_and_coordinates.h"
#include "display.h"
#include "background.h"

extern Adafruit_ST7735 tft;

// ============================================================================
// ФОРМАТИРОВАНИЕ ВРЕМЕНИ
// ============================================================================
inline void formatTime(int seconds, char* buffer) {
  if (seconds < 0) seconds = 0;
  
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = seconds % 60;
  
  char minStr[3], secStr[3];
  
  if (minutes < 10) {
    minStr[0] = '0';
    minStr[1] = '0' + minutes;
    minStr[2] = '\0';
  } else {
    sprintf(minStr, "%d", minutes);
  }
  
  if (secs < 10) {
    secStr[0] = '0';
    secStr[1] = '0' + secs;
    secStr[2] = '\0';
  } else {
    sprintf(secStr, "%d", secs);
  }
  
  if (hours == 0) {
    sprintf(buffer, "%s:%s", minStr, secStr);
  } else {
    sprintf(buffer, "%d:%s:%s", hours, minStr, secStr);
  }
}

inline int getTextWidth(const char* text) {
  int len = strlen(text);
  if (len == 0) return 0;
  // Учитываем отступы между символами (spacingX = 1 в printText)
  return (len * FONT_WIDTH) + ((len - 1) * 1);
}

// ============================================================================
// ОТРИСОВКА ПРОГРЕСС-БАРА
// ============================================================================
inline void drawProgressBar(int currentSec, int totalSec, bool visible, int &previousSliderWidth) {
  if (!visible) {
    int areaHeight = SLIDER_HEIGHT + MARGIN_SMALL + FONT_HEIGHT;
    int startY = PROGRESS_BAR_Y - MARGIN_SMALL - FONT_HEIGHT;
    clearArea(0, startY, TFT_WIDTH, areaHeight, tft_background);
    previousSliderWidth = 0;
    return;
  }
  
  uint16_t lineColor = (totalSec == 0) ? COLOR_PROGRESS_ERROR : COLOR_PROGRESS_NORMAL;
  
  int newSliderWidth = 0;
  if (totalSec > 0) {
    float progress = min((float)currentSec / totalSec, 1.0f);
    newSliderWidth = progress * TFT_WIDTH;
  }
  
  int oldWidth = previousSliderWidth;
  
  if (oldWidth != newSliderWidth) {
    if (oldWidth > newSliderWidth) {
      int sliderY = PROGRESS_BAR_Y - 3;
      for (int j = 0; j < SLIDER_HEIGHT; j++) {
        for (int i = newSliderWidth; i < oldWidth && i < TFT_WIDTH; i++) {
          int pixelIdx = (sliderY + j) * TFT_WIDTH + i;
          uint16_t backColor = pgm_read_word(&tft_background[pixelIdx]);
          tft.drawPixel(i, sliderY + j, backColor);
        }
      }
    }
    
    if (oldWidth == 0) {
      for (int i = 0; i < PROGRESS_BAR_HEIGHT; i++) {
        tft.drawFastHLine(0, PROGRESS_BAR_Y + i, TFT_WIDTH, lineColor);
      }
    }
    
    if (newSliderWidth > 0) {
      int sliderY = PROGRESS_BAR_Y - 3;
      int startDraw = (oldWidth < newSliderWidth) ? oldWidth : 0;
      int endDraw = newSliderWidth;
      
      for (int j = 0; j < SLIDER_HEIGHT; j++) {
        for (int i = startDraw; i < endDraw && i < TFT_WIDTH; i++) {
          tft.drawPixel(i, sliderY + j, COLOR_PROGRESS_NORMAL);
        }
      }
    }
    
    previousSliderWidth = newSliderWidth;
  }
  
  char currentTimeStr[16];
  char totalTimeStr[16];
  
  formatTime(currentSec, currentTimeStr);
  formatTime(totalSec, totalTimeStr);
  
  int timeAreaHeight = FONT_HEIGHT;
  clearArea(0, TIME_CURRENT_Y, TFT_WIDTH, timeAreaHeight, tft_background);
  
  printText(currentTimeStr, TIME_CURRENT_X, TIME_CURRENT_Y, COLOR_TIME_TEXT);
  
  int totalTimeWidth = getTextWidth(totalTimeStr);
  int totalTimeX = TFT_WIDTH - totalTimeWidth - TIME_TOTAL_X_OFFSET;
  printText(totalTimeStr, totalTimeX, TIME_TOTAL_Y, COLOR_TIME_TEXT);
}

#endif // PROGRESS_BAR_H
