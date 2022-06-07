#ifndef _ym_api_h_
#define _ym_api_h_

#include <stdint.h>

typedef enum YM_METHOD {
  YM_WRITE_SAFE,
  YM_WRITE_VIATIMER,
  YM_WRITE_BUSYFLAG,
  YM_WRITE_FORCEBUSY,
  YM_WRITE_count,
  YM_WRITE_SPECIAL = YM_WRITE_count
} YM_METHOD;

extern uint16_t
  count_fail_busy,    // n times YM was busy and never cleared
  count_fail_nobusy,  // n times YM was never busy
  count_fail_badread, // n times YM_STATUS byte contained 1 in any bit 0-6
  count_fail_flap,    // n times Busy made multiple 0->1 transitions.
  count_ok_busy;      // n successful writes where YM was initially "busy"

extern YM_METHOD ym_current_method;

extern unsigned char __fastcall__ ymwrite (unsigned char reg, unsigned char val);

// all of these return 1=fail, 0=success
extern char ymwrite_lock(YM_METHOD);
extern char ymwrite_unlock();
extern char ymwrite_set(YM_METHOD);

// returns 0=false, nonzero=true
extern char ymwrite_islocked();

#endif
