#ifndef _ui_c_
#define _ui_c_

#include "test_api.h"
#include "ymwrite.h"

extern void clear_counters();
extern void handle_input(unsigned char);
extern void draw_screen();
extern void update_counters();
extern void update_test_indicator(test_unit_e, test_unit_e);
extern void update_write_indicator(YM_METHOD, YM_METHOD);

#endif
