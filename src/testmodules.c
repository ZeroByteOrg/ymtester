#include "ym_api.h"
#include "test_api.h"
#include "irq.h"
#include "util.h"
#include <conio.h>
#include <cx16.h>
#include <stdlib.h> // rand()

#define YM_DATA (*(unsigned char*)(0x9f41))

uint16_t nulltest(test_cmd_e);
uint16_t playscale(test_cmd_e);
uint16_t readzero(test_cmd_e);
uint16_t busyflag(test_cmd_e);
uint16_t probebusy(test_cmd_e);
uint16_t ymtimer(test_cmd_e);
uint16_t ctpinread(test_cmd_e);

test_unit modules[TEST_count] = {
  &nulltest,
  &playscale,
  &readzero,
  &busyflag,
  &probebusy,
  &ymtimer,
  &ctpinread
};

// ym_probe_busy.asm
extern unsigned char __fastcall__ ym_probe_busy();


uint16_t nulltest(test_cmd_e) { return 0; }

// playscale is supplied by playscale.c

uint16_t busyflag(test_cmd_e command) {
  if (command == CMD_START) {
    return ymwrite_lock(YM_WRITE_FORCEBUSY);
  }
  if (command == CMD_STOP) {
    return ymwrite_unlock();
  }
  return ymwrite(0x20 + (unsigned char)rand()%0xe0,0);
}

uint16_t readzero(test_cmd_e command) {
  if (command == CMD_START) {
    return ymwrite_lock(YM_WRITE_SPECIAL);
  }
  if (command == CMD_STOP) {
    return ymwrite_unlock();
  }
  if (YM_DATA == 0) return 0;
  ++count_fail_badread;
  return 1;
}

uint16_t probebusy(test_cmd_e command) {
  if (command == CMD_START) {
    return ymwrite_lock(YM_WRITE_SPECIAL);
  }
  if (command == CMD_STOP) {
    return ymwrite_unlock();
  }
  return ym_probe_busy();
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

uint16_t ctpinread (test_cmd_e command) {
  // this module expects physical connections between YM2151 to VIA2:
  //
  //  ____      1k pullup   ______
  // | YM |         |      | VIA2 |
  // |    |8 ---|<--+--- 8 |      |
  // |    |                |      |
  // |    |9 ---|<--+--- 9 |      |
  // |____|         |      |______|
  //            1k pullup
  //
  // The test writes a value 0..4 as a 2bit value on
  // CT1 and CT2 (pins 8 and 9, CT1=lsb). With the above
  // connectivity, the same value should be readable on VIA2
  // data port A bits 6 and 7.

  unsigned char i, val;
  if (command == CMD_START) {
    // set VIA2 DDRA to be inputs on PA6 and PA7
    VIA2.ddra &= 0x3F; // (leave config of pins PA0-5 as they were)
    VIA2.acr &= 0xFE;  // clear bit 0 (set portA latching = disabled)
    return 0;
                  // bits 6-7: T1: one-shot mode/PB7 disabled
                  // bit    5: T2: PB6 disabled (T2 is always a one-shot timer)
                  // bits 2-4: Shift Reg disabled
                  // bit    1: Port B latching disabled
                  // bit    0: Port A latching disabled

  }
  if (command == CMD_STOP) {
    return 0;
  }
  val = VIA2.pra2 & 0xc0; // get current value being output by YM CT pins.
  val = (val + 0x40) & 0xc0; // next desired value to write...
  ymwrite(0x1B, val); // ignore any errors returned by ymwrite()
  for (i=0 ; i<100 ; i++) snooze(); // wait long enough to guarantee pins updated.

  gotoxy(50,50);
  cprintf ("expect:%02x  got:%02x",val,VIA2.pra2 & 0xc0);

  if ((VIA2.pra2 & 0xc0)==val) return 0;
  else return 1;
}
