#ifndef _tests_h_
#define _tests_h_

typedef enum test_module {
  TEST_AUDIBLE,
  TEST_READ_ZERO,
  TEST_READ_STATUS,
  TEST_IRQ,
  TEST_WRITE_HARNESS,
  TEST_count
} test_module;

/*
extern uint16_t test_errcount;
extern char test_state;

extern uint16_t* tests[TEST_count];

extern void test_start();
extern void test_stop();
extern void test_select(test_module);
extern uint16_t* test(char);
*/



#endif
