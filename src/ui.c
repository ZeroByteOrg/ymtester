#include "ui.h"
#include "test_api.h"
#include "ymwrite.h"
#include <conio.h>

#define printval(x) cprintf("%05u\n\r",(x))
#define printcounter(x,name) cprintf("%05u : %s",(x),(name))

void clear_counters() {
  count_fail_busy = 0;
  count_fail_nobusy = 0;
  count_fail_badread = 0;
  count_ok_busy = 0;
  test_errors = 0;
  test_count = 0;
}

void handle_input(unsigned char c) {
  static YM_METHOD previous_method;
  previous_method = ym_current_method;
  gotoxy(50,8);
  cputc(c);
  cprintf(" %u",c);
  if (c>='0' && c<'4') {
    c -= 48;
    if (c != current_test) {
      update_test_indicator(current_test, c);
      test_select(c);
    }
    return;
  }
  switch (c) {
  case 13: // enter
    if (test_state == STATE_RUNNING) test_stop();
    else test_start();
    update_test_indicator(current_test, current_test);
    break;
  case ')':
    ymwrite_set(YM_WRITE_SAFE);
    break;
  case '!':
    ymwrite_set(YM_WRITE_VIATIMER);
    break;
  case '@':
    ymwrite_set(YM_WRITE_BUSYFLAG);
    break;
  case '#':
    ymwrite_set(YM_WRITE_FORCEBUSY);
    break;
  case 'c':
  case 'C':
    clear_counters();
    break;
  }
  if (previous_method != ym_current_method)
    update_write_indicator(previous_method, ym_current_method);
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
  cprintf("Press C to reset counters\n\n\n\n\r");
  cprintf("Press Enter to start/stop current test.\n\n\r");
  cprintf("   0: No Test\n\r");
  cprintf("   1: Audible Music Test\n\r");
  cprintf("   2: Busy Flag Read Test\n\r");
  cprintf("   3: Clean Status Read Test\n\r");
  cprintf("   4: IRQ test (not implemented)\n\r");
  cprintf("   5: Timer Status Flags Test (not iplemented)\n\r");
  cprintf("   6: CT output pin test (not implemented)\n\n\n\r");
  cprintf("Press SHIFT+# to select YM Write Method:\n\n\r");
  cprintf("   0: Safe\n\r");
  cprintf("   1: VIA Timer (will fail)\n\r");
  cprintf("   2: Check Busy Flag\n\r");
  cprintf("   3: Require Busy Flag\n\r");
  cprintf("    : Special (automatic)");
  update_test_indicator(current_test, current_test);
  update_write_indicator(ym_current_method, ym_current_method);
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

void update_test_indicator(test_unit_e old, test_unit_e new) {
  gotoxy(1,16+old);
  cputc(' ');
  gotoxy(1,16+new);
  if (test_state == STATE_RUNNING)
    cputc('>');
  else
    cputc('-');
}

void update_write_indicator(YM_METHOD old, YM_METHOD new) {
  gotoxy (1,27+old);
  cputc(' ');
  gotoxy (1,27+new);
  cputc ('>');
}
