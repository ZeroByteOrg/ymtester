#include "ym_api.h"
#include "test_api.h"
#include "irq.h"
#include "util.h"
#include <conio.h>

#define YM_DATA (*(unsigned char*)(0x9f41))

uint16_t nulltest(test_cmd_e);
uint16_t playscale(test_cmd_e);
uint16_t readzero(test_cmd_e);
uint16_t busyflag(test_cmd_e);
uint16_t probebusy(test_cmd_e);
uint16_t ymtimer(test_cmd_e);

test_unit modules[TEST_count] = {
  &nulltest,
  &playscale,
  &readzero,
  &busyflag,
  &probebusy,
  &playscale,
  &ymtimer,
  &playscale
};

// ym_probe_busy.asm
extern unsigned char __fastcall__ ym_probe_busy();


uint16_t nulltest(test_cmd_e) { return 0; }

// playscale is supplied by playscale.c

uint16_t busyflag(test_cmd_e command) {
  static YM_METHOD lastmethod ;
  if (command == CMD_START) {
    lastmethod = ym_current_method;
    if (ymwrite_set(YM_WRITE_FORCEBUSY) == 0) {
      ymwrite_lock();
      return 0;
    }
    else{
      return 1;
    }
  }
  if (command == CMD_STOP) {
    ymwrite_unlock();
    return ymwrite_set(lastmethod);
  }
  return ymwrite(0,0);
}

uint16_t readzero(test_cmd_e command) {
  static YM_METHOD lastmethod;
  if (command == CMD_START) {
    lastmethod = ym_current_method;
    ymwrite_lock();
    ymwrite_set(YM_WRITE_SPECIAL);
    return 0;
  }
  if (command == CMD_STOP) {
    ymwrite_unlock();
    return ymwrite_set(lastmethod);
  }
  if (YM_DATA == 0) return 0;
  ++count_fail_badread;
  return 1;
}

uint16_t probebusy(test_cmd_e command) {
  static YM_METHOD lastmethod ;
  uint16_t errors;
  if (command == CMD_START) {
    lastmethod = ym_current_method;
    ymwrite_lock();
    ymwrite_set(YM_WRITE_SPECIAL);
    return 0;
  }
  if (command == CMD_STOP) {
    ymwrite_unlock();
    return ymwrite_set(lastmethod);
  }
  errors = ym_probe_busy();
  // TODO: make tests pausable
  // if (errors) test_state = STATE_PAUSE;
  // note - the UI should accomplish this functionality, provided
  // the test API supports pause/resume. UI should call those
  // when it sees the counters change.
  return errors;
}

uint16_t ymtimer(test_cmd_e command) {
  // notes: Min period Ta = 17.9us Tb = 286.1us
  // 1 CPU cycle @ 8MHz = 125ns
  // 1 CPU cycle @ 1MHz = 1us
  // Thus this test should wait at least ~300 CPU cycles to be
  // safe and ensure both clocks have expired
  char j;
  uint16_t errors = 0;
  static char last = 0;
  if (command == CMD_START) {
    // set both timers to the same interval - the fastest possible for CLKB
    errors += ymwrite(0x10, 0xfb); // clkA bits 2-9
    errors += ymwrite(0x11, 0x00); // clkA bits 0-1
    errors += ymwrite(0x12, 0xfe); // clkB
    return errors;
  }
  if (command == CMD_STOP) {
    return 0;
  }
  if(++last > 2) last = 1;
  errors += ymwrite(0x14, 0x3c+last); // clear and sart the timers
  if(errors) return errors;
  ++irq_waiting;
  for (j=0 ; j<16 ; j++) {
    snooze();
    if(irq_waiting==0) break;
  }
  if(irq_waiting) {
    ++errors;
    ++count_fail_flap; // use that counter, as it's only used by pollbusy module.
    irq_waiting=0;
    return errors;
  }
  gotoxy(50,50);
  cprintf ("expect:%02x  got:%02x",last,irq_ym_status);
  if ((irq_ym_status&0x7f) != last) {
    // ignore the busy flag as an error
    ++errors;
    // any non-timer-bit being set is a dirty read
    if (irq_ym_status & 0xfc) ++count_fail_badread;
  }
  return errors;
}
