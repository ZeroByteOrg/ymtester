//#define _main_c_
//#include <stdio.h>
#include <conio.h>
#include <cx16.h>
#include "ui.h"
#include "test_api.h"
#include "ym_api.h"
#include "util.h"       // vsync()
#include "irq.h"


void init() {
  VIA2.ddra = 0;
  VIA2.ddrb = 0;
  VIA2.acr  = 0; // T1,T2 one-shot, shift disa, PA&PB latch disabled
  VIA2.pcr  = 0; // default handshake controls
  VIA2.ier  = 0; // disable all interrupts

  // Initialize VIA2, Timer 1 as a one-shot interval timer to stand in for the
  // YM busy flag if the YM is unreadable.

  // Computed min number of CPU clocks for safe YM busy:
  // 64 * 8m / 3.579545m = 143.0349 CPU clocks @ 8mhz
  // Real HW has been observed to take up to ~1.38uSec before busy flag active...
  // (11 CPU ticks at 8mhz)
  // So the default pause time is chosen as 155.
  VIA2.t1l_lo = 155;
  VIA2.t1l_hi = 0;

  irq_install();
  ymwrite_unlock();
  ymwrite_set(YM_WRITE_SAFE);
  test_select(TEST_AUDIBLE);
  test_init();
  draw_screen();
}

int main() {
//  unsigned char key;
  init();
  while (1) {
    vsync();
    if(kbhit()) handle_input(cgetc());
    test_run();
    update_screen();
  }
  irq_remove();
  return 0;
}
