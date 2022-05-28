#ifndef _ymwrite_h_
#define _ymwrite_h_

#include <stdint.h>

typedef enum YM_METHOD {
  YM_WRITE_SAFE,
  YM_WRITE_VIATIMER,
  YM_WRITE_BUSYFLAG
} YM_METHOD;

#ifdef _main_c_
uint16_t ym_timeouts, ym_nobusy, ym_badstatus;
#else
extern uint16_t ym_timeouts, ym_nobusy, ym_badstatus;
#endif

extern unsigned char __fastcall__ ymwrite (unsigned char reg, unsigned char val);
extern unsigned char __fastcall__ ymwrite_setmethod (YM_METHOD);

#endif
