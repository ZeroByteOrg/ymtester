#include "unit_test.h"

extern unsigned char __fastcall__ ym_testbusy();

uint16_t busyflag(test_cmd_e command) {

  if (command == CMD_STOP) return 0;
  return ym_testbusy();
}
