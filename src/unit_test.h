#ifndef _unit_test_h_
#define _unit_test_h_

#include <stdint.h>

typedef enum test_unit_e {
  TEST_NULL,
  TEST_AUDIBLE,
  TEST_READ_ZERO,
  TEST_READ_STATUS,
  TEST_IRQ,
  TEST_WRITE_HARNESS,
  TEST_count
} test_unit_e;

typedef enum test_state_e {
  STATE_IDLE,
  STATE_RUNNING
} test_state_e;

typedef enum test_cmd_e {
  CMD_STOP = -1,
  CMD_RUN = 0,
  CMD_START = 1
} test_cmd_e;

typedef uint16_t(*test_unit)(test_cmd_e);

extern uint16_t test_errors;

extern void test_select(test_unit_e);
extern void test_start();
extern void test_stop();
extern void test_check();

#endif
