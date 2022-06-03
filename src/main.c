//#define _main_c_
//#include <stdio.h>
#include <conio.h>
#include <cx16.h>
#include "ui.h"
#include "test_api.h"
#include "ymwrite.h"
#include "vsync.h"


void init() {
  ymwrite_unlock();
  ymwrite_set(YM_WRITE_BUSYFLAG);
  test_select(TEST_NULL);
}

int main() {
//  unsigned char key;
  init();
  draw_screen();
  while (1) {
    vsync();
    if(kbhit()) handle_input(cgetc());
    test_check();
    update_screen();
  }
  return 0;
}
