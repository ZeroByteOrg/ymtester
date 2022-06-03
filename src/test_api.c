#include "test_api.h"
#include "testmodules.h"

char test_number;

test_unit modules[TEST_count] = {
  &nulltest,
  &playscale,
  &readzero,
  &busyflag,
  &probebusy,
  &playscale,
  &playscale,
  &playscale
};

test_unit test_module=&nulltest;

test_state_e test_state  = STATE_IDLE;
test_unit_e current_test = TEST_NULL;

uint16_t test_errors = 0;
uint16_t test_count  = 0;

void test_select(test_unit_e t) {
  test_state_e state = test_state;
  if (state == STATE_RUNNING) test_stop();
  asm("sei");
  test_module = modules[t];
  asm("cli");
  if (state == STATE_RUNNING) test_start();
  current_test = t;
}

void test_start() {
  test_count = 1;
  test_state = STATE_RUNNING;
  test_errors += test_module(CMD_START);
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
