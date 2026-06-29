#ifndef TIMINGS_H
#define TIMINGS_H

#include <Arduino.h>

// ============================================================================
// ТАЙМИНГИ ПОДКЛЮЧЕНИЯ И ИНИЦИАЛИЗАЦИИ
// ============================================================================
const unsigned long WIFI_CONNECT_TIMEOUT = 400;       // Таймаут попытки подключения WiFi (мс)
const int WIFI_CONNECT_ATTEMPTS = 15;                 // Количество попыток подключения WiFi
const unsigned long SERVER_CONNECT_TIMEOUT = 1000;    // Таймаут подключения к серверу (мс)
const unsigned long AUTH_TIMEOUT = 1000;              // Таймаут авторизации (мс)
const unsigned long STATUS_DISPLAY_DELAY_OK = 1000;   // Задержка показа статуса OK (мс)
const unsigned long STATUS_DISPLAY_DELAY_ERROR = 1000;// Задержка показа статуса error (мс)
const unsigned long FINAL_STATUS_DELAY = 3000;        // Задержка перед переходом на главный экран (мс)

// ============================================================================
// ТАЙМИНГИ РАБОТЫ СИСТЕМЫ
// ============================================================================
const unsigned long QUERY_INTERVAL = 1000;            // Интервал опроса Kodi (мс)
const unsigned long INIT_RETRY_DELAY = 1000;          // Интервал между попытками инициализации (мс)
const unsigned long PRESSED_ICON_DURATION = 2000;     // Время показа иконки нажатой кнопки (мс)

// ============================================================================
// ТАЙМИНГИ АВТОПОВТОРА КНОПОК
// ============================================================================
const unsigned long REPEAT_INTERVAL_FIXED = 500;     // Фиксированный интервал для каналов (мс)
const unsigned long REPEAT_PHASE_1_DURATION = 5000;   // Длительность первой фазы ускорения (мс)
const unsigned long REPEAT_PHASE_1_INTERVAL = 500;   // Интервал первой фазы (мс)
const unsigned long REPEAT_PHASE_2_DURATION = 5000;  // Длительность второй фазы ускорения (мс)
const unsigned long REPEAT_PHASE_2_INTERVAL = 300;    // Интервал второй фазы (мс)
const unsigned long REPEAT_PHASE_3_INTERVAL = 150;    // Интервал третьей фазы (мс)


// ============================================================================
// ТАЙМИНГИ РЕЖИМА ЧАСОВ
// ============================================================================
const unsigned long CLOCK_IDLE_TIMEOUT = 300000;    // Время бездействия для активации часов (5 минут)
const unsigned long CLOCK_PAUSE_TIMEOUT = 1800000;  // Время паузы для активации часов (30 минут)
const unsigned long CLOCK_UPDATE_INTERVAL = 60000;  // Интервал обновления часов (1 минута)
const unsigned long NTP_SYNC_INTERVAL = 600000;    // Интервал синхронизации с NTP (1 час)
#endif // TIMINGS_H
