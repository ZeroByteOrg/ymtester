#include "test_api.h"
#include "testmodules.h"

#include <conio.h> // for debug output - DELME

//test_unit test_module=&nulltest;
test_unit test_module;


test_state_e test_state  = STATE_IDLE;
test_module_e next_test  = TEST_NULL;
test_module_e current_test = TEST_NULL;

uint16_t test_errors = 0;
uint16_t test_count  = 0;
char     autostart   = 0;

void test_select(test_module_e t) {
  next_test = t;
  if (test_state >= STATE_STARTING)
    autostart = 1;
  else
    autostart = 0;
}

void test_start() {
  if ((test_state == STATE_PAUSED)||(test_state==STATE_RUNNING))
    test_state = STATE_RUNNING;
  else
    autostart = 1;
}

void test_stop() {
  autostart = 0;
  if (test_state > STATE_STOPPING) test_state = STATE_STOPPING;
}

void test_run() {

  uint16_t e=0; // for capturing errors during starting/stopping states

  while(kbhit()) cgetc();
  gotoxy(40,21);
  cprintf("Tcur:%02x Tnxt:%02x state:%02x as:%02x",current_test, next_test, test_state,autostart);
//  if (test_state!= STATE_IDLE) while(!kbhit()) {}

  if (current_test != next_test) {
    switch (test_state) {
      case STATE_STARTING:
      case STATE_RUNNING:
      case STATE_PAUSED:
        test_state = STATE_STOPPING;
        break;
      case STATE_IDLE:
        asm("sei");
        test_module = modules[next_test];
        asm("cli");
        current_test = next_test;
        break;
    }
  }
  if (autostart && test_state == STATE_IDLE) {
    test_state = STATE_STARTING;
    test_count = 0;
    autostart = 0;
  }
  switch (test_state) {
    case STATE_IDLE:
    case STATE_PAUSED:
      break;
    case STATE_RUNNING:
      ++test_count;
      test_errors += test_module(CMD_RUN);
      break;
    case STATE_STARTING:
      ++test_count;
      e = test_module(CMD_START);
      if (e==0)
        test_state = STATE_RUNNING;
      else
        test_errors += e;
      break;
    case STATE_STOPPING:
    case STATE_STOPPING2:
    case STATE_FORCING_STOP:
      ++test_count;
      e = test_module(CMD_STOP);
      if (e==0) {
        test_state = STATE_IDLE;
      }
      else {
        test_errors += e;
        --test_state;
      }
      break;
  }
}
