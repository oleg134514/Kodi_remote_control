#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "config.h"
#include "color_and_coordinates.h"
#include "font_7x14.h"
#include "font_12x24_digits.h"

extern Adafruit_ST7735 tft;
extern const uint8_t degreeSymbol[];

// ============================================================================
// ПОИСК ГЛИФОВ В ШРИФТАХ
// ============================================================================
inline const uint8_t* getGlyphData(uint16_t code) {
  int left = 0, right = GLYPH_COUNT - 1;
  while (left <= right) {
    int mid = left + (right - left) / 2;
    uint16_t midCode = pgm_read_word(&glyphs[mid].code);
    if (midCode == code) {
      return (const uint8_t*)pgm_read_ptr(&glyphs[mid].data);
    }
    if (midCode < code) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }
  return nullptr;
}

inline const uint8_t* getDigitGlyphData(uint16_t code) {
  int left = 0, right = GLYPH_COUNT_DIGITS_12x24 - 1;
  while (left <= right) {
    int mid = left + (right - left) / 2;
    uint16_t midCode = pgm_read_word(&glyphs_DIGITS_12x24[mid].code);
    if (midCode == code) {
      return (const uint8_t*)pgm_read_ptr(&glyphs_DIGITS_12x24[mid].data);
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
// ДЕКОДИРОВАНИЕ UTF-8
// ============================================================================
inline uint32_t utf8_decode(const char* &str) {
  uint8_t c = *str;
  if (c == 0) return 0;
  if (c < 0x80) { str++; return c; } 
  else if ((c & 0xE0) == 0xC0) {
    uint32_t code = ((c & 0x1F) << 6) | (str[1] & 0x3F); str += 2; return code;
  } else if ((c & 0xF0) == 0xE0) {
    uint32_t code = ((c & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F); str += 3; return code;
  } else {
    uint32_t code = ((c & 0x07) << 18) | ((str[1] & 0x3F) << 12) | ((str[2] & 0x3F) << 6) | (str[3] & 0x3F); str += 4; return code;
  }
}

// ============================================================================
// ОТРИСОВКА ТЕКСТА
// ============================================================================
inline void printText(const char* text, int startX, int startY, uint16_t color) {
  int x = startX, y = startY;
  int gw = FONT_WIDTH, gh = FONT_HEIGHT;
  int spacingX = 1, spacingY = 1;
  
  while (*text) {
    uint32_t code = utf8_decode(text);
    if (code == 0) break;
    if (code == '\n') { x = startX; y += gh + spacingY; continue; }
    if (code == '\r') continue;
    
    const uint8_t* bitmap = getGlyphData((uint16_t)code);
    if (!bitmap) { x += gw + spacingX; continue; }
    
    if (x + gw > TFT_WIDTH) { x = startX; y += gh + spacingY; }
    if (y + gh > TFT_HEIGHT) break;
    
    tft.drawBitmap(x, y, bitmap, gw, gh, color);
    x += gw + spacingX;
  }
}

// ============================================================================
// ОЧИСТКА ОБЛАСТИ ЭКРАНА
// ============================================================================
inline void clearArea(int x, int y, int width, int height, const uint16_t* background) {
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      int pixelIdx = (y + j) * TFT_WIDTH + (x + i);
      uint16_t backColor = pgm_read_word(&background[pixelIdx]);
      tft.drawPixel(x + i, y + j, backColor);
    }
  }
}

#endif // DISPLAY_H
