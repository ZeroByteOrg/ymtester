#include "ymwrite.h"

char ym_locked = 0;

extern unsigned char __fastcall__ ymwrite_setmethod (YM_METHOD);

void ymwrite_lock() {
  ym_locked = 1;
}

void ymwrite_unlock() {
  ym_locked = 0;
}

char ymwrite_set(YM_METHOD method) {
  if ((method == YM_WRITE_SPECIAL) && (ym_locked != 0)) {
    ym_current_method = YM_WRITE_SPECIAL; // only affects display
    return 0;
  }
  if (ym_locked != 0) return 1;
  if (method >= YM_WRITE_count) return 1;
  return ymwrite_setmethod (method);
}

char ymwrite_islocked() {
  return (ym_locked != 0);
}
