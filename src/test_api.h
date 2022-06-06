#ifndef _test_api_h_
#define _test_api_h_

#include <stdint.h>

typedef enum test_module_e {
  TEST_NULL,
  TEST_AUDIBLE,
  TEST_READ_STATUS,
  TEST_BUSY_FLAG,
  TEST_BUSY_PROBE,
  TEST_IRQ,
  TEST_TIMERS,
  TEST_WRITE_HARNESS,
  TEST_count
} test_module_e;


typedef enum test_state_e {
  STATE_IDLE,
  STATE_FORCING_STOP,
  STATE_STOPPING2,
  STATE_STOPPING,
  STATE_STARTING,
  STATE_RUNNING,
  STATE_PAUSED,
  STATE_count
} test_state_e;

typedef enum test_cmd_e {
  CMD_STOP = -1,
  CMD_RUN = 0,
  CMD_START = 1
} test_cmd_e;

typedef uint16_t(*test_unit)(test_cmd_e);

extern uint16_t test_errors, test_count;
extern test_state_e test_state;
extern test_module_e current_test;


extern void test_select(test_module_e);
extern void test_start();
extern void test_stop();
extern void test_run();

#endif
