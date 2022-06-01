//#define _main_c_
//#include <stdio.h>
#include <conio.h>
#include <cx16.h>
#include "unit_test.h"
#include "ymwrite.h"
#include "vsync.h"

#define printval(x) cprintf("%05u\n\r",(x))
#define printcounter(x,name) cprintf("%05u : %s",(x),(name))

char test_number;

void init() {
  test_number = 0;
  ymwrite_setmethod(YM_WRITE_BUSYFLAG);
  test_select(TEST_NULL);
}

void clear_counters() {
  count_fail_busy = 0;
  count_fail_nobusy = 0;
  count_fail_badread = 0;
  count_ok_busy = 0;
  test_errors = 0;
  test_count = 0;
}

void handle_input(unsigned char c) {
  gotoxy(50,8);
  cputc(c);
  cprintf(" %u",c);
  if (c>='0' && c<'4') {
    gotoxy(1,14+test_number);
    cputc(' ');
    test_number = c-48;
    gotoxy(1,14+test_number);
    cputc('>');
    test_select(test_number);
    return;
  }
  switch (c) {
  case 13: // enter
    if (test_state == STATE_RUNNING) test_stop();
    else test_start();
    break;
  case 'c':
  case 'C':
    clear_counters();
    break;
  }
}

void draw_screen() {
  clrscr();
  gotoxy(0,0);
  cprintf("YM READ/WRITE STATS:\n\r");
  printcounter(0,"Busy Timeout\n\r");
  printcounter(0,"Never Busy\n\r");
  printcounter(0,"Dirty Reads\n\r");
  printcounter(0,"Success Busy Wait\n\n\r");
  cprintf("TEST MODULE ERRORS:\n\r");
  printcounter(0, "Errors\n\r");
  printcounter(0, "Iterations\n\n\r");
  cprintf("Press C to reset counters\n\n\r");
  cprintf("Press Enter to start/stop current test.\n\n\r");
  cprintf("   0: No Test\n\r");
  cprintf("   1: Audible Music Test\n\r");
  cprintf("   2: Busy Flag Read Test\n\r");
  cprintf("   3: Clean Status Read Test\n\r");
  cprintf("   4: IRQ test (not implemented)\n\r");
  cprintf("   5: Timer Status Flags Test (not iplemented)\n\r");
  cprintf("   6: CT output pin test (not implemented)\n\n\n\r");
}

void update_counters() {
  gotoxy(0,1);
  printval(count_fail_busy);
  printval(count_fail_nobusy);
  printval(count_fail_badread);
  printval(count_ok_busy);
  cprintf("\n\n");
  printval(test_errors);
  printval(test_count);
}

int main() {
//  unsigned char key;
  init();
  draw_screen();
  ymwrite_setmethod(YM_WRITE_BUSYFLAG);
  handle_input('1');
  test_start();
  while (1) {
    vsync();
    if(kbhit()) handle_input(cgetc());
    test_check();
    update_counters();
  }
  return 0;
}
