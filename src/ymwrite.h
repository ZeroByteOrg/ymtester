#ifndef _ymwrite_h_
#define _ymwrite_h_

#include <stdint.h>

typedef enum YM_METHOD {
  YM_WRITE_SAFE,
  YM_WRITE_VIATIMER,
  YM_WRITE_BUSYFLAG
} YM_METHOD;

extern uint16_t
  ym_timeouts,  // number of times YM was busy and never cleared
  ym_nobusy,    // number of times YM was immediately "available" when writing
  ym_yesbusy,   // number of times YM was initially "busy" when writing
  ym_badstatus; // number of times YM_STATUS byte contained 1 in any bit 0-6

extern unsigned char __fastcall__ ymwrite (unsigned char reg, unsigned char val);
extern unsigned char __fastcall__ ymwrite_setmethod (YM_METHOD);

#endif
