#include "ymwrite.h"
#include "unit_test.h"

uint16_t busyflag(test_cmd_e command) {
  static YM_METHOD lastmethod ;
  if (command == CMD_START) {
    lastmethod = ym_current_method;
    return ymwrite_setmethod(YM_WRITE_FORCEBUSY);
  }
  if (command == CMD_STOP) return ymwrite_setmethod(lastmethod);
  return 0;
  return ymwrite(0,0);
}
