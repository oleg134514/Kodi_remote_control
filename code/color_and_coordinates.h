#ifndef COLOR_AND_COORDINATES_H
#define COLOR_AND_COORDINATES_H

#include <Arduino.h>
#include "font_24x48_digits.h"
#include "font_36x72_digits.h"
#include "symbol.h"

// ============================================================================
// РАЗМЕРЫ ЭКРАНА И ЭЛЕМЕНТОВ
// ============================================================================
const int TFT_WIDTH = 160;              // Ширина экрана в пикселях
const int TFT_HEIGHT = 128;             // Высота экрана в пикселях
const int ICON_SIZE = 48;               // Размер иконок (квадрат) в пикселях
const int FONT_WIDTH = 7;               // Ширина символа шрифта 7x14
const int FONT_HEIGHT = 14;             // Высота символа шрифта 7x14
const int DIGIT_WIDTH = 12;             // Ширина цифры шрифта 12x24
const int DIGIT_HEIGHT = 24;            // Высота цифры шрифта 12x24

// ============================================================================
// ЦВЕТА ЭЛЕМЕНТОВ ИНТЕРФЕЙСА
// Формат: 0xRRGGBB (16-bit RGB565)
// ============================================================================

// --- Цвета текста статусов подключения ---
const uint16_t COLOR_STATUS_TEXT = 0xFFFF;      // Белый цвет для текста статусов
const uint16_t COLOR_STATUS_OK = 0x07E0;        // Зеленый цвет для статуса "OK"
const uint16_t COLOR_STATUS_ERROR = 0xF800;     // Красный цвет для статуса "error"

// --- Цвета CPU и температуры ---
const uint16_t COLOR_CPU_TEMP_LABEL = 0xFFFF;   // Белый цвет для надписей "CPU=" и "T="
const uint16_t COLOR_CPU_TEMP_VALUE = 0xFFFF;   // Белый цвет для цифр и символов (%, °, C)

// --- Критические значения и цвета для CPU ---
const int CPU_CRITICAL_THRESHOLD = 80;          // Порог критической загрузки CPU (%)
const uint16_t COLOR_CPU_CRITICAL = 0xF800;     // Красный цвет при критической загрузке

// --- Критические значения и цвета для температуры ---
const float TEMP_CRITICAL_THRESHOLD = 70.0;     // Порог критической температуры (°C)
const uint16_t COLOR_TEMP_CRITICAL = 0xF800;    // Красный цвет при критической температуре

// --- Цвета громкости ---
const uint16_t COLOR_VOLUME_NORMAL = 0xFFFF;    // Белый цвет для цифр громкости (нормально)
const uint16_t COLOR_VOLUME_MUTED = 0x8410;     // Серый цвет для цифр громкости (при mute)

// --- Цвета прогресс-бара ---
const uint16_t COLOR_PROGRESS_NORMAL = 0xFFFF;  // Белый цвет для прогресс-бара (нормально)
const uint16_t COLOR_PROGRESS_ERROR = 0xF800;   // Красный цвет при totalSec = 0

// --- Цвета времени на прогресс-баре ---
const uint16_t COLOR_TIME_TEXT = 0xFFFF;        // Белый цвет для времени

// ============================================================================
// КООРДИНАТЫ ЭЛЕМЕНТОВ ИНТЕРФЕЙСА
// ============================================================================

// --- Статус воспроизведения (левый верхний угол) ---
const int STATUS_X = 4;                         // X координата статуса воспроизведения
const int STATUS_Y = 4;                         // Y координата статуса воспроизведения

// --- Иконка громкости ---
const int VOLUME_ICON_X = STATUS_X + ICON_SIZE + 4;  // X координата иконки громкости
const int VOLUME_ICON_Y = STATUS_Y;                  // Y координата иконки громкости

// --- Цифры громкости ---
const int VOLUME_DIGITS_X = VOLUME_ICON_X + ICON_SIZE + 4;  // X координата цифр громкости
const int VOLUME_DIGITS_Y = 16;                              // Y координата цифр громкости

// --- Иконка нажатой кнопки ---
const int PRESSED_ICON_X = 4;                     // X координата иконки нажатой кнопки
const int PRESSED_ICON_Y = 56;                    // Y координата иконки нажатой кнопки

// --- CPU (отдельная координата) ---
const int CPU_X = 56;                             // X координата для CPU
const int CPU_Y = 56;                             // Y координата для CPU

// --- Температура (отдельная координата) ---
const int TEMP_X = CPU_X;                         // X координата для температуры
const int TEMP_Y = CPU_Y + FONT_HEIGHT + 1;       // Y координата для температуры

// --- Прогресс-бар ---
const int PROGRESS_BAR_Y = 119;                   // Y координата прогресс-бара
const int PROGRESS_BAR_HEIGHT = 3;                // Высота базовой линии прогресс-бара
const int SLIDER_HEIGHT = 8;                      // Высота ползунка прогресс-бара

// --- Время на прогресс-баре ---
const int TIME_CURRENT_X = 4;                     // X координата текущего времени
const int TIME_CURRENT_Y = PROGRESS_BAR_Y - 4 - FONT_HEIGHT;  // Y координата текущего времени

const int TIME_TOTAL_X_OFFSET = 4;                // Отступ от правого края для общего времени
const int TIME_TOTAL_Y = TIME_CURRENT_Y;          // Y координата общего времени

// ============================================================================
// ОТСТУПЫ МЕЖДУ ЭЛЕМЕНТАМИ
// ============================================================================
const int MARGIN_SMALL = 4;                       // Малый отступ (4 пикселя)
const int MARGIN_MEDIUM = 8;                      // Средний отступ (8 пикселей)
const int MARGIN_LARGE = 16;                      // Большой отступ (16 пикселей)

// ============================================================================
// ЦВЕТА ЧАСОВ И ДАТЫ
// ============================================================================
const uint16_t COLOR_CLOCK_TIME = 0xFFFF;           // Белый цвет для времени (NTP)
const uint16_t COLOR_CLOCK_DATE = 0xFFFF;           // Белый цвет для даты (NTP)
const uint16_t COLOR_CLOCK_TIME_INTERNAL = 0xF800;  // Красный цвет для времени (внутренние часы)
const uint16_t COLOR_CLOCK_DATE_INTERNAL = 0xFD20;  // Оранжевый цвет для даты (внутренние часы)

// ============================================================================
// КООРДИНАТЫ ЧАСОВ И ДАТЫ
// ============================================================================

// Время (шрифт 36x72)
const int CLOCK_HOURS_X = 0;                       // X координата часов
const int CLOCK_HOURS_Y = 56;                       // Y координата часов
const int CLOCK_COLON_X = CLOCK_HOURS_X + (FONT_WIDTH_DIGITS_36x72 * 2);  // X координата двоеточия
const int CLOCK_COLON_Y = CLOCK_HOURS_Y;            // Y координата двоеточия
const int CLOCK_MINUTES_X = CLOCK_COLON_X + COLON_WIDTH;  // X координата минут
const int CLOCK_MINUTES_Y = CLOCK_HOURS_Y;          // Y координата минут

// Дата (шрифт 24x48)
const int CLOCK_DAY_X = 0;                         // X координата дней
const int CLOCK_DAY_Y = 4;                          // Y координата дней
const int CLOCK_SLASH_X = CLOCK_DAY_X + (FONT_WIDTH_DIGITS_24x48 * 2);  // X координата слеша
const int CLOCK_SLASH_Y = CLOCK_DAY_Y;              // Y координата слеша
const int CLOCK_MONTH_X = CLOCK_SLASH_X + SLASH_WIDTH;  // X координата месяцев
const int CLOCK_MONTH_Y = CLOCK_DAY_Y;              // Y координата месяцев

#endif // COLOR_AND_COORDINATES_H
