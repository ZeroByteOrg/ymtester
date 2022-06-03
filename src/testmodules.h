#ifndef _testmodules_h_
#define _testmodules_h_

#include "test_api.h"
#include <stdint.h>

extern uint16_t nulltest(test_cmd_e);
extern uint16_t playscale(test_cmd_e);
extern uint16_t readzero(test_cmd_e);
extern uint16_t busyflag(test_cmd_e);
extern uint16_t probebusy(test_cmd_e);

#endif
