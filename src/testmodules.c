#include "ymwrite.h"
#include "test_api.h"

#define YM_DATA (*(unsigned char*)(0x9f41))

uint16_t nulltest(test_cmd_e) { return 0; }

// playscale is supplied by playscale.c

uint16_t busyflag(test_cmd_e command) {
  static YM_METHOD lastmethod ;
  if (command == CMD_START) {
    lastmethod = ym_current_method;
    return ymwrite_setmethod(YM_WRITE_FORCEBUSY);
  }
  if (command == CMD_STOP) return ymwrite_setmethod(lastmethod);
  return ymwrite(0,0);
}

uint16_t readzero(test_cmd_e command) {
  if (command == CMD_START) return 0;
  if (command == CMD_STOP) return 0;
  if (YM_DATA == 0) return 0;
  else return 1;
}
