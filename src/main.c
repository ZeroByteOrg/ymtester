#define _main_c_
#include <stdio.h>
#include "unit_test.h"
#include "ymwrite.h"
#include "vsync.h"

void init() {
  ymwrite_setmethod(YM_WRITE_BUSYFLAG);
}

int main() {
  ymwrite_setmethod(YM_WRITE_BUSYFLAG);
  if (ymwrite(0,0)) printf("Something went wrong.");
  else printf("It worked.");
  test_select(TEST_AUDIBLE);
  test_start();
  while (1) {
    vsync();
    test_check();
  }
  return 0;
}
