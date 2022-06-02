#include "unit_test.h"

#define YM_DATA (*(unsigned char*)(0x9f41))

uint16_t readzero(test_cmd_e command) {
  if (command == CMD_START) return 0;
  if (command == CMD_STOP) return 0;
  if (YM_DATA == 0) return 0;
  else return 1;
}
