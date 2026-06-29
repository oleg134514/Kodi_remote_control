#ifndef KODI_API_H
#define KODI_API_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "timings.h"

// ============================================================================
// ОТПРАВКА ПАКЕТНОГО ЗАПРОСА К KODI
// ============================================================================
inline bool sendKodiBatchRequest(const char* payload, JsonDocument& doc) {
  HTTPClient http;
  String url = String("http://") + KODI_IP + ":" + KODI_PORT + "/jsonrpc";
  
  http.begin(url);
  http.setAuthorization(KODI_USER, KODI_PASS);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(SERVER_CONNECT_TIMEOUT);
  
  int httpCode = http.POST(payload);
  if (httpCode == HTTP_CODE_OK) {
    DeserializationError error = deserializeJson(doc, http.getStream());
    http.end();
    return !error;
  }
  http.end();
  return false;
}

// ============================================================================
// ОТПРАВКА КОМАНДЫ В KODI
// ============================================================================
inline void sendKodiCommand(const char* command) {
  HTTPClient http;
  String url = String("http://") + KODI_IP + ":" + KODI_PORT + "/jsonrpc";
  
  http.begin(url);
  http.setAuthorization(KODI_USER, KODI_PASS);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(SERVER_CONNECT_TIMEOUT);
  
  http.POST(command);
  http.end();
}

// ============================================================================
// ПАРСИНГ ТЕМПЕРАТУРЫ
// ============================================================================
inline float parseTemperature(const char* tempStr) {
  if (!tempStr || strlen(tempStr) == 0) return -1.0;
  
  float temp = 0.0;
  bool found = false;
  
  for (int i = 0; tempStr[i] != '\0'; i++) {
    if ((tempStr[i] >= '0' && tempStr[i] <= '9') || tempStr[i] == '.' || tempStr[i] == '-') {
      char numStr[16];
      int j = 0;
      while (tempStr[i] != '\0' && ((tempStr[i] >= '0' && tempStr[i] <= '9') || 
             tempStr[i] == '.' || tempStr[i] == '-')) {
        numStr[j++] = tempStr[i++];
      }
      numStr[j] = '\0';
      temp = atof(numStr);
      found = true;
      break;
    }
  }
  
  return found ? temp : -1.0;
}
// ============================================================================
// ПАРСИНГ ЗАГРУЗКИ CPU (для формата "#0: 0.0% #1: 16% ...")
// ============================================================================
inline int parseCpuLoad(JsonVariant cpuVariant) {
  if (cpuVariant.isNull()) return 0;
  
  // Если это число — возвращаем как есть
  if (cpuVariant.is<int>()) {
    return cpuVariant.as<int>();
  }
  
  if (cpuVariant.is<float>()) {
    float value = cpuVariant.as<float>();
    if (value < 1.0 && value > 0.0) {
      return (int)(value * 100);
    }
    return (int)value;
  }
  
  // Если это строка
  if (cpuVariant.is<const char*>()) {
    const char* cpuStr = cpuVariant.as<const char*>();
    if (!cpuStr || strlen(cpuStr) == 0) return 0;
    
    Serial.print("[parseCpuLoad] Parsing: ");
    Serial.println(cpuStr);
    
    // Извлекаем все числа из строки формата "#0: 0.0% #1: 16% ..."
    float sum = 0.0;
    int count = 0;
    
    int i = 0;
    while (cpuStr[i] != '\0') {
      // Ищем начало числа (цифра или точка)
      while (cpuStr[i] != '\0' && !((cpuStr[i] >= '0' && cpuStr[i] <= '9') || cpuStr[i] == '.')) {
        i++;
      }
      
      if (cpuStr[i] == '\0') break;
      
      // Извлекаем число
      char numStr[16];
      int j = 0;
      while (cpuStr[i] != '\0' && j < 15 && 
             ((cpuStr[i] >= '0' && cpuStr[i] <= '9') || cpuStr[i] == '.')) {
        numStr[j++] = cpuStr[i++];
      }
      numStr[j] = '\0';
      
      if (j > 0) {
        float value = atof(numStr);
        sum += value;
        count++;
        Serial.print("  Found value: ");
        Serial.println(value);
      }
    }
    
    if (count > 0) {
      int avgLoad = (int)(sum / count + 0.5); // Округление
      Serial.print("  Average: ");
      Serial.print(sum);
      Serial.print(" / ");
      Serial.print(count);
      Serial.print(" = ");
      Serial.println(avgLoad);
      return avgLoad;
    }
    
    // Если не нашли ни одного числа, пробуем старый метод (первое число)
    char numStr[16];
    int j = 0;
    for (int k = 0; cpuStr[k] != '\0' && j < 15; k++) {
      if ((cpuStr[k] >= '0' && cpuStr[k] <= '9') || cpuStr[k] == '.') {
        numStr[j++] = cpuStr[k];
      } else if (j > 0) {
        break;
      }
    }
    numStr[j] = '\0';
    
    if (j > 0) {
      float value = atof(numStr);
      if (value < 1.0 && value > 0.0) {
        return (int)(value * 100);
      }
      return (int)value;
    }
  }
  
  return 0;
}
#endif // KODI_API_H
