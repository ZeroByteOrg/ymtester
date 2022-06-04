//#define _main_c_
//#include <stdio.h>
#include <conio.h>
#include <cx16.h>
#include "ui.h"
#include "test_api.h"
#include "ym_api.h"
#include "vsync.h"


void init() {
  ymwrite_unlock();
  ymwrite_set(YM_WRITE_BUSYFLAG);
  test_select(TEST_NULL);
  draw_screen();
}

int main() {
//  unsigned char key;
  init();
  while (1) {
    vsync();
    if(kbhit()) handle_input(cgetc());
    test_check();
    update_screen();
  }
  return 0;
}
