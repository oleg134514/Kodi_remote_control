#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include "config.h"
#include "color_and_coordinates.h"
#include "timings.h"
#include "display.h"
#include "connection.h"
#include "ir_control.h"
#include "kodi_api.h"
#include "progress_bar.h"
#include "ui_elements.h"
#include "background.h"
#include "bitmaps.h"
#include "bitmaps_data.h"
#include "clock.h"

// ============================================================================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
// ============================================================================
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RES);

bool systemReady = false;
unsigned long lastQueryTime = 0;
unsigned long lastInitAttempt = 0;

unsigned long pressedIconTimer = 0;
bool pressedIconVisible = false;

ButtonRepeatState repeatState = {0, 0, 0, false};

DisplayState currentState = {0, -1, false, -1, -1.0, 0, 0, false, 0};
DisplayState previousState = {-1, -1, false, -1, -1.0, -1, -1, false, 0};

ClockState clockState = {false, false, 0, 0, 0, 0, 0, 0, 0, 0};

// ============================================================================
// ОБРАБОТКА ИК-КОМАНДЫ
// ============================================================================
void handleIRCommand(uint32_t code, bool isRepeat) {
  // Сбрасываем таймер взаимодействия при любом нажатии
  resetInteractionTimer();
  
  // Если активен режим часов — выходим из режима и показываем обычный UI
  if (clockState.isClockModeActive) {
    clockState.isClockModeActive = false;
    // Перерисовываем весь UI
    tft.drawRGBBitmap(0, 0, tft_background, TFT_WIDTH, TFT_HEIGHT);
    previousState = {-1, -1, false, -1, -1.0, -1, -1, false, 0};
    drawPlaybackStatus();
    drawVolumeDisplay();
    drawCpuAndTemp();
    drawProgressBar(currentState.currentSec, currentState.totalSec, 
                   currentState.progressBarVisible, currentState.previousSliderWidth);
  }
  
  const IrCommand* cmd = nullptr;
  
  for (int i = 0; i < irCommandsCount; i++) {
    if (irCommands[i].code == code) {
      cmd = &irCommands[i];
      break;
    }
  }
  
  if (!cmd) return;
  
  drawPressedIcon(code);
  pressedIconTimer = millis();
  pressedIconVisible = true;
  
  unsigned long now = millis();
  
  // --- Специальная обработка Play/Pause ---
  if (code == 0xFB04FD00 && !isRepeat) {
    const char* playPauseCmd = nullptr;
    
    switch (currentState.playbackStatus) {
      case 0:  // STOP
      case 2:  // Пауза
      case 3:  // Перемотка вперед
      case 4:  // Перемотка назад
        playPauseCmd = R"({"jsonrpc":"2.0","method":"Input.ExecuteAction","params":{"action":"play"},"id":1})";
        break;
      case 1:  // Воспроизведение
        playPauseCmd = R"({"jsonrpc":"2.0","method":"Input.ExecuteAction","params":{"action":"pause"},"id":1})";
        break;
      default:
        playPauseCmd = R"({"jsonrpc":"2.0","method":"Input.ExecuteAction","params":{"action":"play"},"id":1})";
        break;
    }
    
    sendKodiCommand(playPauseCmd);
    return;
  }
  
  // --- Специальная обработка Screen Toggle (локальная, без отправки в Kodi) ---
  if (code == 0xED12FD00 && !isRepeat) {
    // Кнопка только для локальной индикации, команда в Kodi не отправляется
    return;
  }
  
  // --- Стандартная обработка остальных кнопок ---
  if (cmd->repeatType != REPEAT_NONE) {
    if (!isRepeat) {
      repeatState.lastCode = code;
      repeatState.firstPressTime = now;
      repeatState.lastCommandTime = now;
      repeatState.isActive = true;
      
      if (cmd->kodiCommand != nullptr) {
        sendKodiCommand(cmd->kodiCommand);
      }
    } else if (repeatState.lastCode == code && repeatState.isActive) {
      unsigned long holdDuration = now - repeatState.firstPressTime;
      unsigned long interval = calculateRepeatInterval(holdDuration, cmd->repeatType);
      
      if (now - repeatState.lastCommandTime >= interval) {
        if (cmd->kodiCommand != nullptr) {
          sendKodiCommand(cmd->kodiCommand);
        }
        repeatState.lastCommandTime = now;
      }
    }
  } else {
    if (!isRepeat) {
      if (cmd->kodiCommand != nullptr) {
        sendKodiCommand(cmd->kodiCommand);
      }
    }
  }
}

// ============================================================================
// ОБНОВЛЕНИЕ ДАННЫХ KODI
// ============================================================================
void updateKodiData() {
  if (!systemReady) return;
  
  const char* batchPayload = R"([
    {"jsonrpc":"2.0","method":"Application.GetProperties","params":{"properties":["version","volume","muted"]},"id":1},
    {"jsonrpc":"2.0","method":"XBMC.GetInfoLabels","params":{"labels":["System.CpuUsage","System.CPUTemperature"]},"id":2},
    {"jsonrpc":"2.0","method":"Player.GetActivePlayers","id":3}
  ])";

  JsonDocument doc;
  if (sendKodiBatchRequest(batchPayload, doc)) {
    JsonArray responses = doc.as<JsonArray>();
    
    for (JsonObject resp : responses) {
      int id = resp["id"];
      
      if (id == 1) {
        JsonObject result = resp["result"];
        currentState.volume = result["volume"].as<int>();
        currentState.muted = result["muted"].as<bool>();
      } 
      else if (id == 2) {
        JsonObject result = resp["result"];
        
        // Используем System.CpuUsage (работает!)
        JsonVariant cpuUsage = result["System.CpuUsage"];
        if (!cpuUsage.isNull() && strlen(cpuUsage.as<const char*>()) > 0) {
          currentState.cpuLoad = parseCpuLoad(cpuUsage);
        } else {
          currentState.cpuLoad = 0;
        }
        
        // Температура
        JsonVariant tempVar = result["System.CPUTemperature"];
        if (!tempVar.isNull()) {
          const char* tempStr = tempVar.as<const char*>();
          currentState.temperature = parseTemperature(tempStr);
        } else {
          currentState.temperature = -1.0;
        }
      } 
      else if (id == 3) {
        JsonArray players = resp["result"];
        
        if (players.size() == 0) {
          currentState.playbackStatus = 0;
          currentState.progressBarVisible = false;
        } else {
          int playerId = players[0]["playerid"];
          
          String playerPayload = String(R"({"jsonrpc":"2.0","method":"Player.GetProperties","params":{"playerid":)") + 
                                 playerId + 
                                 R"(,"properties":["speed","time","totaltime"]},"id":4})";
                                 
          JsonDocument pDoc;
          if (sendKodiBatchRequest(playerPayload.c_str(), pDoc)) {
            JsonObject pRes = pDoc["result"];
            int speed = pRes["speed"];
            
            if (speed == 0) {
              currentState.playbackStatus = 2;
            } else if (speed == 1) {
              currentState.playbackStatus = 1;
            } else if (speed > 1) {
              currentState.playbackStatus = 3;
            } else if (speed < 0) {
              currentState.playbackStatus = 4;
            }
            
            currentState.progressBarVisible = true;
            
            int curH = pRes["time"]["hours"], curM = pRes["time"]["minutes"], curS = pRes["time"]["seconds"];
            int totH = pRes["totaltime"]["hours"], totM = pRes["totaltime"]["minutes"], totS = pRes["totaltime"]["seconds"];
            
            currentState.currentSec = (curH * 3600) + (curM * 60) + curS;
            currentState.totalSec = (totH * 3600) + (totM * 60) + totS;
          }
        }
      }
    }
    
    // Перерисовка UI только если НЕ активен режим часов
    if (!clockState.isClockModeActive) {
      if (currentState.playbackStatus != previousState.playbackStatus) {
        drawPlaybackStatus();
      }

      if (currentState.volume != previousState.volume || currentState.muted != previousState.muted) {
        drawVolumeDisplay();
      }

      if (currentState.cpuLoad != previousState.cpuLoad || currentState.temperature != previousState.temperature) {
        drawCpuAndTemp();
      }

      if (currentState.progressBarVisible != previousState.progressBarVisible ||
          currentState.currentSec != previousState.currentSec ||
          currentState.totalSec != previousState.totalSec) {
        drawProgressBar(currentState.currentSec, currentState.totalSec, 
                       currentState.progressBarVisible, currentState.previousSliderWidth);
      }
    }

    previousState = currentState;
  } else {
    systemReady = false;
  }
}

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(500);
  
  pinMode(IR_RECEIVER_PIN, INPUT);
  IrReceiver.begin(IR_RECEIVER_PIN, DISABLE_LED_FEEDBACK);
  
  pinMode(TFT_RES, OUTPUT);
  digitalWrite(TFT_RES, HIGH); delay(50);
  digitalWrite(TFT_RES, LOW); delay(100);
  digitalWrite(TFT_RES, HIGH); delay(100);
  
  SPI.begin(TFT_SCK, -1, TFT_SDA, TFT_CS);
  tft.setSPISpeed(20000000);
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  
  if (performSequencedInitialization()) {
    systemReady = true;
    
    currentState = {0, -1, false, -1, -1.0, 0, 0, false, 0};
    previousState = {-1, -1, false, -1, -1.0, -1, -1, false, 0};
    drawPlaybackStatus();
    drawVolumeDisplay();
    drawCpuAndTemp();
    drawProgressBar(currentState.currentSec, currentState.totalSec, 
                   currentState.progressBarVisible, currentState.previousSliderWidth);
    
    // Инициализация NTP после успешного подключения
    initNTP();
    clockState.lastInteractionTime = millis();
  } else {
    systemReady = false;
  }
  lastInitAttempt = millis();
}

// ============================================================================
// LOOP
// ============================================================================
void loop() {
  if (!systemReady && (millis() - lastInitAttempt >= INIT_RETRY_DELAY)) {
    if (performSequencedInitialization()) {
      systemReady = true;
      
      currentState = {0, -1, false, -1, -1.0, 0, 0, false, 0};
      previousState = {-1, -1, false, -1, -1.0, -1, -1, false, 0};
      drawPlaybackStatus();
      drawVolumeDisplay();
      drawCpuAndTemp();
      drawProgressBar(currentState.currentSec, currentState.totalSec, 
                     currentState.progressBarVisible, currentState.previousSliderWidth);
      
      initNTP();
      clockState.lastInteractionTime = millis();
    } else {
      systemReady = false;
    }
    lastInitAttempt = millis();
  }

  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.protocol == NEC) {
      uint32_t code = IrReceiver.decodedIRData.decodedRawData;
      bool isRepeat = (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT);
      
      if (code != 0x0 || isRepeat) {
        if (isRepeat && code == 0x0) {
          code = repeatState.lastCode;
        }
        
        if (code != 0x0) {
          handleIRCommand(code, isRepeat);
        }
      }
    }
    IrReceiver.resume();
  }
  
  if (pressedIconVisible && (millis() - pressedIconTimer >= PRESSED_ICON_DURATION)) {
    if (!clockState.isClockModeActive) {
      clearArea(PRESSED_ICON_X, PRESSED_ICON_Y, ICON_SIZE, ICON_SIZE, tft_background);
    }
    pressedIconVisible = false;
  }

  if (systemReady && (millis() - lastQueryTime >= QUERY_INTERVAL)) {
    lastQueryTime = millis();
    updateKodiData();
  }
  
  // Обновление режима часов
  updateClock(currentState.playbackStatus);
}
