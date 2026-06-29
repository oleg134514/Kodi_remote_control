#ifndef IR_CONTROL_H
#define IR_CONTROL_H

#include <Arduino.h>
#include <IRremote.hpp>
#include "config.h"
#include "color_and_coordinates.h"
#include "timings.h"
#include "bitmaps.h"

// ============================================================================
// ТИПЫ АВТОПОВТОРА
// ============================================================================
enum RepeatType {
  REPEAT_NONE,
  REPEAT_FIXED,
  REPEAT_ACCELERATED
};

// ============================================================================
// СТРУКТУРА ИК-КОМАНДЫ
// ============================================================================
struct IrCommand {
  uint32_t code;
  const uint16_t* bitmap;
  const char* kodiCommand;
  RepeatType repeatType;
};

// ============================================================================
// ТАБЛИЦА ИК-КОМАНД
// ============================================================================
const IrCommand irCommands[] = {
  {0xEA15FD00, color_1, R"({"jsonrpc":"2.0","method":"Input.SendText","params":{"text":"1"},"id":1})", REPEAT_NONE},
  {0xE916FD00, color_2, R"({"jsonrpc":"2.0","method":"Input.SendText","params":{"text":"2"},"id":1})", REPEAT_NONE},
  {0xE817FD00, color_3, R"({"jsonrpc":"2.0","method":"Input.SendText","params":{"text":"3"},"id":1})", REPEAT_NONE},
  {0xE619FD00, color_4, R"({"jsonrpc":"2.0","method":"Input.SendText","params":{"text":"4"},"id":1})", REPEAT_NONE},
  {0xE51AFD00, color_5, R"({"jsonrpc":"2.0","method":"Input.SendText","params":{"text":"5"},"id":1})", REPEAT_NONE},
  {0xE41BFD00, color_6, R"({"jsonrpc":"2.0","method":"Input.SendText","params":{"text":"6"},"id":1})", REPEAT_NONE},
  {0xE21DFD00, color_7, R"({"jsonrpc":"2.0","method":"Input.SendText","params":{"text":"7"},"id":1})", REPEAT_NONE},
  {0xE11EFD00, color_8, R"({"jsonrpc":"2.0","method":"Input.SendText","params":{"text":"8"},"id":1})", REPEAT_NONE},
  {0xE01FFD00, color_9, R"({"jsonrpc":"2.0","method":"Input.SendText","params":{"text":"9"},"id":1})", REPEAT_NONE},
  {0xBE41FD00, color_0, R"({"jsonrpc":"2.0","method":"Input.SendText","params":{"text":"0"},"id":1})", REPEAT_NONE},
  {0xDF20FD00, color_vol_plus, R"({"jsonrpc":"2.0","method":"Application.SetVolume","params":{"volume":"increment"},"id":1})", REPEAT_ACCELERATED},
  {0xDE21FD00, color_vol_minus, R"({"jsonrpc":"2.0","method":"Application.SetVolume","params":{"volume":"decrement"},"id":1})", REPEAT_ACCELERATED},
  {0xDD22FD00, color_mute, R"({"jsonrpc":"2.0","method":"Application.SetMute","params":{"mute":"toggle"},"id":1})", REPEAT_NONE},
  {0xE718FD00, color_ch_plus, R"({"jsonrpc":"2.0","method":"Input.ExecuteAction","params":{"action":"channelup"},"id":1})", REPEAT_FIXED},
  {0xE31CFD00, color_ch_minus, R"({"jsonrpc":"2.0","method":"Input.ExecuteAction","params":{"action":"channeldown"},"id":1})", REPEAT_FIXED},
  {0xA55AFD00, color_rewind, R"({"jsonrpc":"2.0","method":"Input.ExecuteAction","params":{"action":"rewind"},"id":1})", REPEAT_NONE},
  {0xFB04FD00, color_play_pause, nullptr, REPEAT_NONE},  // Обработка в handleIRCommand
  {0xF10EFD00, color_forward, R"({"jsonrpc":"2.0","method":"Input.ExecuteAction","params":{"action":"fastforward"},"id":1})", REPEAT_NONE},
  {0xF40BFD00, color_up, R"({"jsonrpc":"2.0","method":"Input.Up","id":1})", REPEAT_ACCELERATED},
  {0xF00FFD00, color_down, R"({"jsonrpc":"2.0","method":"Input.Down","id":1})", REPEAT_ACCELERATED},
  {0xB649FD00, color_left, R"({"jsonrpc":"2.0","method":"Input.Left","id":1})", REPEAT_ACCELERATED},
  {0xB54AFD00, color_right, R"({"jsonrpc":"2.0","method":"Input.Right","id":1})", REPEAT_ACCELERATED},
  {0xF20DFD00, color_ok, R"({"jsonrpc":"2.0","method":"Input.Select","id":1})", REPEAT_NONE},
  {0xFD02FD00, color_back, R"({"jsonrpc":"2.0","method":"Input.Back","id":1})", REPEAT_NONE},
  {0xF807FD00, color_menu, R"({"jsonrpc":"2.0","method":"Input.ExecuteAction","params":{"action":"activatewindow","params":["home"]},"id":1})", REPEAT_NONE},
  {0xA758FD00, color_context_menu, R"({"jsonrpc":"2.0","method":"Input.ContextMenu","id":1})", REPEAT_NONE},
  {0x9F60FD00, color_info, R"({"jsonrpc":"2.0","method":"Input.Info","id":1})", REPEAT_NONE},
  {0xED12FD00, color_screen_toggle, nullptr, REPEAT_NONE}
};
const int irCommandsCount = sizeof(irCommands) / sizeof(irCommands[0]);

// ============================================================================
// СОСТОЯНИЕ АВТОПОВТОРА
// ============================================================================
struct ButtonRepeatState {
  uint32_t lastCode;
  unsigned long firstPressTime;
  unsigned long lastCommandTime;
  bool isActive;
};

extern ButtonRepeatState repeatState;

// ============================================================================
// РАСЧЕТ ИНТЕРВАЛА АВТОПОВТОРА
// ============================================================================
inline unsigned long calculateRepeatInterval(unsigned long holdDuration, RepeatType repeatType) {
  if (repeatType == REPEAT_FIXED) {
    return REPEAT_INTERVAL_FIXED;
  }
  
  if (repeatType == REPEAT_ACCELERATED) {
    if (holdDuration < REPEAT_PHASE_1_DURATION) {
      return REPEAT_PHASE_1_INTERVAL;
    } else if (holdDuration < REPEAT_PHASE_2_DURATION) {
      return REPEAT_PHASE_2_INTERVAL;
    } else {
      return REPEAT_PHASE_3_INTERVAL;
    }
  }
  
  return 0;
}

#endif // IR_CONTROL_H
