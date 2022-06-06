#include "ui.h"
#include "test_api.h"
#include "ym_api.h"
#include <conio.h>

#define printval(x) cprintf("%05u\n\r",(x))
#define printcounter(x,name) cprintf("%05u : %s",(x),(name))

#define busyview ((char*)0xA000)
#define VIEW_LEN 32
#define ram_bank (*(unsigned char*) 0x0000)

void draw_busyview();
void update_counters();
void update_busyview();
void update_test_indicator();
void update_write_indicator();

static const char ruler[] = "]-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+[";


void clear_ym_counters() {
  count_fail_busy = 0;
  count_fail_nobusy = 0;
  count_fail_badread = 0;
  count_ok_busy = 0;
  count_fail_flap = 0;
}

void clear_test_counters() {
  test_errors = 0;
  test_count = 0;
}

void clear_counters() {
  clear_ym_counters();
  clear_test_counters();
}

void handle_input(unsigned char c) {
  static YM_METHOD previous_method;
  previous_method = ym_current_method;
#if(0)
  gotoxy(50,28);
  cputc(c);
  cprintf(" %u",c);
#endif
  if (c>='0' && c<TEST_count+48) {
//    c -= 48;
//    if (c != current_test) {
//      update_test_indicator(current_test, c);
      test_select(c-48);
//    }
//    c += 48;
  }
  switch (c) {
  case 13: // enter
    switch (test_state) {
      case STATE_PAUSED:
      case STATE_IDLE:
        test_start();
        break;
      default:
        test_stop();
    }
    break;
  case 'a':
  case 'A':
    ymwrite_set(YM_WRITE_SAFE);
    break;
  case 'b':
  case 'B':
    ymwrite_set(YM_WRITE_VIATIMER);
    break;
  case 'c':
  case 'C':
    ymwrite_set(YM_WRITE_BUSYFLAG);
    break;
  case 'd':
  case 'D':
    ymwrite_set(YM_WRITE_FORCEBUSY);
    break;
  case 'p':
  case 'P':
    test_toggle_autopause();
    break;
  case 'w':
  case 'W':
    clear_ym_counters();
    break;
  case 't':
  case 'T':
    clear_test_counters();
    break;
  }
//  update_write_indicator(previous_method, ym_current_method);
}

void draw_screen() {
  clrscr();
  gotoxy(0,0);
  cprintf("YM READ/WRITE STATS:\n\r");
  printcounter(0,"Busy Timeout\n\r");
  printcounter(0,"Never Busy\n\r");
  printcounter(0,"Dirty Reads\n\r");
  printcounter(0,"Busy Flaps\n\r");
  printcounter(0,"OK Busy\n\n\r");
  cprintf("TEST MODULE ERRORS:\n\r");
  printcounter(0, "Errors\n\r");
  printcounter(0, "Iterations\n\n\r");
  cprintf("Press W to reset [W]rite counters\n\n\n\n\r");
  cprintf("Press Enter to start/stop current test.\n\n\r");
  cprintf("   0: No Test\n\r");
  cprintf("   1: Audible Music Test\n\r");
  cprintf("   2: Clean Status Read Test\n\r");
  cprintf("   3: Busy Flag Read Test\n\r");
  cprintf("   4: Busy Flag Probe\n\r");
  cprintf("   5:  X \n\r");
  cprintf("   6: Timer Status Flags Test\n\r");
  cprintf("   7: CT output pin test (not implemented)\n\n\n\r");
  cprintf("Select YM Write Method:\n\n\r");
  cprintf("   A: Safe\n\r");
  cprintf("   B: VIA Timer (will fail)\n\r");
  cprintf("   C: Check Busy Flag\n\r");
  cprintf("   D: Require Busy Flag\n\r");
  cprintf("    : Special (automatic)");
  draw_busyview();
  update_screen();
}

void draw_busyview() {
  unsigned char i;
  ram_bank = 1;
  for(i=0;i<VIEW_LEN;i++) busyview[i]='-';
  update_busyview();
}

void update_screen() {
  update_counters();
  update_busyview();
  update_test_indicator();
  update_write_indicator();
}

void update_busyview() {
  busyview[VIEW_LEN] = 0; // ensure null-termination
  gotoxy(30,4);
  ram_bank = 1;
  cprintf("%32s",busyview);
}

void update_counters() {
  gotoxy(0,1);
  printval(count_fail_busy);
  printval(count_fail_nobusy);
  printval(count_fail_badread);
  printval(count_fail_flap);
  printval(count_ok_busy);
  cprintf("\n\n");
  printval(test_errors);
  printval(test_count);
}

void update_test_indicator() {
  static test_module_e  prev_test = TEST_count;
  static test_state_e prev_state = STATE_count;

  if (current_test == prev_test && test_state == prev_state) return;
  gotoxy(1,17+prev_test);
  cputc(' ');
  gotoxy(1,17+current_test);
  prev_test = current_test;
  prev_state = test_state;
  if (test_state == STATE_RUNNING)
    cputc('>');
  else if (test_state == STATE_PAUSED)
    cputc('+');
  else
    cputc('-');
}

void update_write_indicator() {
  static YM_METHOD prev_method = YM_WRITE_count;
  static char prev_lock = -1;

  if (ymwrite_islocked() == prev_lock && ym_current_method == prev_method) return;

  gotoxy (1,29+prev_method);
  cputc(' ');
  gotoxy (1,29+ym_current_method);
  prev_method = ym_current_method;
  prev_lock = ymwrite_islocked();
  if (prev_lock)
    cputc ('*');
  else
    cputc ('>');
}
