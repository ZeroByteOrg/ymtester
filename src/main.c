//#define _main_c_
//#include <stdio.h>
#include <conio.h>
#include <cx16.h>
#include "unit_test.h"
#include "ymwrite.h"
#include "vsync.h"

#define printval(x) cprintf("%05u\n\r",(x))
#define printcounter(x,name) cprintf("%05u : %s",(x),(name))

void init() {

  ymwrite_setmethod(YM_WRITE_SAFE);
  test_select(TEST_NULL);
}

void draw_screen() {
  clrscr();
  gotoxy(0,0);
  cprintf("YM READ/WRITE STATS:\n\r");
  printcounter(0,"Writes Busy\n\r");
  printcounter(0,"Writes Not Busy\n\r");
  printcounter(0,"Busy Timeouts\n\r");
  printcounter(0,"Dirty Reads\n\n\r");
  cprintf("TEST MODULE ERRORS:\n\r");
  printval(0);
}

void update_counters() {
  gotoxy(0,1);
  printval(ym_yesbusy);
  printval(ym_nobusy);
  printval(ym_timeouts);
  printval(ym_badstatus);
  cprintf("\n\n");
  printval(test_errors);
}

int main() {
  init();
  draw_screen();
  ymwrite_setmethod(YM_WRITE_BUSYFLAG);
  test_select(TEST_AUDIBLE);
  test_start();
  while (1) {
    vsync();
    test_check();
    update_counters();
  }
  return 0;
}
