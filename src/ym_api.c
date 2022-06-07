#include "ym_api.h"

char ym_locked = 0;

YM_METHOD lastmethod;

extern unsigned char __fastcall__ ymwrite_setmethod (YM_METHOD);

char ymwrite_lock(YM_METHOD method) {
  if (! ym_locked) {
    if (method != YM_WRITE_SPECIAL)
      if (ymwrite_set(method)==0) {
        ym_current_method = method;
        ym_locked = 1;
        return 0;
      }
      else return 1;
    else {
      lastmethod = ym_current_method;
      ym_current_method = YM_WRITE_SPECIAL;
      ym_locked = 1;
      return 0;
    }
  }
  return 1;
}

char ymwrite_unlock() {
  if (ym_locked) {
    if (ymwrite_setmethod(lastmethod) == 0) {
      ym_current_method = lastmethod;
      ym_locked = 0;
      return 0;
    }
    else return 1;
  }
  return 0; // already unlocked.
}

char ymwrite_set(YM_METHOD method) {
  if (ym_locked) return 1;
  if (method >= YM_WRITE_count) return 1;
  return ymwrite_setmethod (method);
}

char ymwrite_islocked() {
  return (ym_locked != 0);
}
