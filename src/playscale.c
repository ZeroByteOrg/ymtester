
#include "ym_api.h"
#include "test_api.h"
#include <stdint.h>

static const unsigned char tone[] = {
        0xc4,0x00,0x02,0x62,0x32,0x62,0x23,0x23,0x0a,0x0a,0x12,0x12,0x12,0x12,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x06,0x06
};

// song format: delay , reg , val , ...
// delay 0xff = end

static const unsigned char CMAJOR_SCALE[] = {
  00,0x08,0x00,  00,0x28+0,0x2e,  00,0x08,0x78+0,  07,0x08,0x00,
  00,0x08,0x01,  00,0x28+1,0x31,  00,0x08,0x78+1,  07,0x08,0x01,
  00,0x08,0x02,  00,0x28+2,0x34,  00,0x08,0x78+2,  07,0x08,0x02,
  00,0x08,0x03,  00,0x28+3,0x35,  00,0x08,0x78+3,  07,0x08,0x03,
  00,0x08,0x04,  00,0x28+4,0x38,  00,0x08,0x78+4,  07,0x08,0x04,
  00,0x08,0x05,  00,0x28+5,0x3a,  00,0x08,0x78+5,  07,0x08,0x05,
  00,0x08,0x06,  00,0x28+6,0x3d,  00,0x08,0x78+6,  07,0x08,0x06,
  00,0x08,0x07,  00,0x28+7,0x3e,  00,0x08,0x78+7,  07,0x08,0x07,
  20,0x28,0x2e,   00,0x28+1,0x34, 00,0x28+2,0x38, 00,0x28+3,0x3e,
  00,0x08,0x78+0, 00,0x08,0x78+1, 00,0x08,0x78+2, 00,0x08,0x78+3,
  07,0x08,0,      00,0x08,1,      00,0x08,2,      00,0x08,3,
  00,0x08,0x78+0, 00,0x08,0x78+1, 00,0x08,0x78+2, 00,0x08,0x78+3,
  60,0x08,0,      00,0x08,1,      00,0x08,2, 00,0x08,3,
  0xff
};

uint16_t yminit();
uint16_t ympatch(unsigned char, const unsigned char*);
//uint16_t playscale();

uint16_t yminit() {
  unsigned char voice, reg;
  uint16_t errors = 0;
  for (voice=0;voice<8;voice++) {
    reg=0xe0+voice;
    // set RR=max on all 4 OPs and then release the voice
    errors += ymwrite(reg+0x00,0x0f);
    errors += ymwrite(reg+0x08,0x0f);
    errors += ymwrite(reg+0x10,0x0f);
    errors += ymwrite(reg+0x18,0x0f);
    errors += ymwrite(0x08,voice);
    // load "tone" patch into voice
    errors += ympatch(voice,tone);
  }
  errors += ymwrite(0x0f,0); //Noise register
  errors += ymwrite(0x14,0); //CTRL register
  errors += ymwrite(0x18,0); // LFO frequency
  errors += ymwrite(0x19,0); // AMD
  errors += ymwrite(0x19,0x80); // PMD
  errors += ymwrite(0x1b,0); // Waveform / CT output pins
  return errors;
}

uint16_t ympatch(unsigned char v, const unsigned char* patch) {
  uint8_t i;
  uint16_t errors = 0;
  errors += ymwrite(0x20+v,*patch);
  ++patch;
  i = 0x38+v;
  while (i>=0x30) {
    errors += ymwrite(i,*patch);
    ++patch;
    i+=8;
  }
  return errors;
}

uint16_t playscale(test_cmd_e command) {
  static unsigned char delay = 0;
  static const unsigned char* data = CMAJOR_SCALE;
  uint16_t errors = 0;

  if (command == CMD_STOP) { // stop
    delay = 0;
    return yminit();
  }
  if (command == CMD_START) { // start
    delay = 1;
    data = CMAJOR_SCALE;
    errors = yminit();
  }
  // continue
  if (delay == 0) return 0;
  if (--delay > 0) return 0;
  while (delay == 0) {
    errors += ymwrite(data[1], data[2]);
    data = data+3;
    delay = data[0];
    if (delay==0xff) {
      data = CMAJOR_SCALE;
      delay = 60 + data[0];
    }
  }
  return errors;
}
