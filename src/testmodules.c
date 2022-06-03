#include "ymwrite.h"
#include "test_api.h"

#define YM_DATA (*(unsigned char*)(0x9f41))

// ym_probe_busy.asm
extern unsigned char __fastcall__ ym_probe_busy();

uint16_t nulltest(test_cmd_e) { return 0; }

// playscale is supplied by playscale.c

uint16_t busyflag(test_cmd_e command) {
  static YM_METHOD lastmethod ;
  if (command == CMD_START) {
    lastmethod = ym_current_method;
    if (ymwrite_set(YM_WRITE_FORCEBUSY) == 0) {
      ymwrite_lock();
      return 0;
    }
    else{
      return 1;
    }
  }
  if (command == CMD_STOP) {
    ymwrite_unlock();
    return ymwrite_set(lastmethod);
  }
  return ymwrite(0,0);
}

uint16_t readzero(test_cmd_e command) {
  if (command == CMD_START) return 0;
  if (command == CMD_STOP) return 0;
  if (YM_DATA == 0) return 0;
  else return 1;
}

uint16_t probebusy(test_cmd_e command) {
  static YM_METHOD lastmethod ;
  uint16_t errors;
  if (command == CMD_START) {
    lastmethod = ym_current_method;
    ymwrite_lock();
    ymwrite_set(YM_WRITE_SPECIAL);
    return 0;
  }
  if (command == CMD_STOP) {
    ymwrite_unlock();
    return ymwrite_set(lastmethod);
  }
  errors = ym_probe_busy();
  // TODO: make tests pausable
  // if (errors) test_state = STATE_PAUSE;
  return errors;
}
