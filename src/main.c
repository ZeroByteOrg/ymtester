#define _main_c_
#include <stdio.h>
#include "ymwrite.h"

#include "playscale.h"

void init() {
  ymwrite_setmethod(YM_WRITE_BUSYFLAG);

}

int main() {
  ymwrite_setmethod(YM_WRITE_BUSYFLAG);
  if (ymwrite(0,0)) printf("Something went wrong.");
  else printf("It worked.");
  playscale();
  return 0;
}
