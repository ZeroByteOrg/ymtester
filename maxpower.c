#include <cbm.h>
#include <conio.h>

int psgfreq[16] = {
  1170, 1172, 1174, 1176, 1178, 1281, 1182, 1184,
  1186, 1188, 1190, 1192, 1194, 1196, 1198, 1200
};

char psgwave[4] = {
  0x3F,  // pulse
  0x40,  // sawtooth
  0x80,  // triangle
  0xC0   // noise
};

char* wavename[4] = {
  "square",
  "sawtooth",
  "triangle",
  "noise"
};

int i;
char psg = 0;
char pcm = 0;
char fm = 1;
char wave = 0;
char fmreset = 0;

void ymwrite(char r, char v) {
  while (YM2151.data & 0x80) {}
  YM2151.reg = r;
  __asm__ ("NOP");
  YM2151.data = v;
}

void psgwrite(char r, char v) {
  VERA.address = 0xF9C0 + r;
  VERA.address_hi = 1 | VERA_INC_1;
  VERA.data0 = v;
}

void fill_fifo() {
  static char i = 0;
  static char sample = 0x7f;
  while((signed char)VERA.audio.control >= 0) {
    VERA.audio.data = sample;
    i = ++i % 16;
    sample = 0x7F + i/8;
  }
}

void ymplay() {
  char i;
  for (i=0 ; i<8 ; i++) ymwrite(8,0x78+i);
}

void psgplay() {
  char i;
  for (i=0 ; i<16 ; i++) psgwrite(4*i+2,0xff);
  for (i=0 ; i<16 ; i++) psgwrite(4*i+3,psgwave[wave]);
}

void pcmplay() {
  fill_fifo();
  VERA.audio.control = 0x0F;
  VERA.audio.rate = 0x19;
}

void ymsilence() {
  char i;
  for(i=0 ; i<8 ; i++) ymwrite(8,i);
}

void psgsilence() {
  char i;
  for (i=0;i<16;i++) psgwrite(4*i+2,0);
}

void pcmsilence() {
  VERA.audio.rate = 0;
  VERA.audio.control = 0x80;
}

void drawscreen() {
  clrscr();
  cprintf("f1: ");
  if(fm) revers(1);
  cprintf("fm ");
  revers(0);
  cprintf("  [space]: ");
  if(fmreset) revers(1);
  cprintf("reset");
  revers(0);
  cprintf("\n\rf2: ");
  if(psg) revers(1);
  cprintf("psg");
  revers(0);
  cprintf("        w: %s\n\r",wavename[wave]);
  cprintf("f3: ");
  if(pcm) revers(1);
  cprintf ("pcm");
  revers(0);
  cprintf ("\n\r Q to quit\n\r");
}

void main () {
//  int f = 300;
  char run = 1;

  for (i=0 ; i<256 ; i++) {
    ymwrite(i,0);
    if (i < 8) ymwrite(8,i);
  }

  for (i=0 ; i<8 ; i++) {
    ymwrite(0x20+i,0xc7);
    ymwrite(0x28+i,0x4A);
    ymwrite(0x40+i,1);
    ymwrite(0x48+i,1);
    ymwrite(0x50+i,1);
    ymwrite(0x58+i,1);
    ymwrite(0x80+i,0x1F);
    ymwrite(0x88+i,0x1F);
    ymwrite(0x90+i,0x1F);
    ymwrite(0x98+i,0x1F);
    ymwrite(0xe0+i,0x0F);
    ymwrite(0xe8+i,0x0F);
    ymwrite(0xf0+i,0x0F);
    ymwrite(0xf8+i,0x0F);
  }

  VERA.audio.control = 0x0F; // max volume, mono 8bit

  for (i=0; i<16 ; i++) {
    psgwrite(4*i+0,psgfreq[5]&0xff);
    psgwrite(4*i+2,0);
    psgwrite(4*i+3,psgwave[wave]);
  }
  VERA.address = 0xF9C1;
  VERA.address_hi = 1 | VERA_INC_4;
  __asm__ ("LDA #%b",4);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);
  __asm__ ("STA %w",0x9F23);

  if (fm) ymplay();
  if (psg) psgplay();
  if (pcm) pcmplay();
  drawscreen();
  while (run) {
    fill_fifo();
    if (kbhit()) {
      switch (cgetc()) {
        case CH_F1:
          if (fm)
            ymsilence();
          else
            ymplay();
          fm = fm ? 0 : 1;
          break;
        case CH_F2:
          if (psg)
            psgsilence();
          else
            psgplay();
          psg = psg ? 0 : 1;
          break;
        case CH_F3:
          if (pcm)
            pcmsilence();
          else
            pcmplay();
          pcm = pcm ? 0 : 1;
          break;
        case 'w':
        case 'W':
          wave = ++wave % 4;
          if (psg) psgplay();
          break;
        case ' ':
          if (fmreset) {
            ymwrite(0x01,0);
            fmreset = 0;
          }
          else {
            ymwrite(0x01,(1<<3));
            fmreset = 1;
          }
          break;
        case 'q':
        case CH_STOP:
        case 'Q':
          cprintf("byebye\n\r");
          run = 0;
          break;
      }
      if(run) drawscreen();
    }
  }
  if (kbhit()) cgetc(); // consume keystroke
  pcmsilence();
  ymsilence();
  psgsilence();

}
