#include "unit_test.h"
#include "testmodules.h"

uint16_t nulltest(test_cmd_e) { return 0; }

test_unit modules[TEST_count] = {
  &nulltest,
  &playscale,
  &busyflag,
  &readzero,
  &playscale,
  &playscale
};

test_unit test_module=&nulltest;

uint8_t  test_state  = STATE_IDLE;
uint16_t test_errors = 0;
uint16_t test_count  = 0;

void test_select(test_unit_e t) {
  if (test_state == STATE_RUNNING)
    test_errors += test_module(CMD_STOP);
  asm("sei");
  test_module = modules[t];
  asm("cli");
  if (test_state == STATE_RUNNING) {
    test_errors += test_module(CMD_START);
    test_count = 1;
  }
}

void test_start() {
  test_count = 1;
  test_state = STATE_RUNNING;
  test_errors = test_module(CMD_START);
}

void test_stop() {
  test_state = STATE_IDLE;
  test_errors += test_module(CMD_STOP);
}

void test_check() {
  if (test_state != STATE_RUNNING) return;
  ++test_count;
  test_errors += test_module(CMD_RUN);
}
