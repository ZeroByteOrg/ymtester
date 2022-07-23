#include <conio.h>
#include <cbm.h>
#include <stdio.h>

#define PSGBASE 0xF9C0

extern void __fastcall__ vsync();

char ympatch[26] = {
  0xC7, 0x00,
  0x01, 0x01, 0x01, 0x01,
  0x00, 0x00, 0x00, 0x00,
  0x1F, 0x1F, 0x1F, 0x1F,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0xFF, 0xFF
};

enum noteval {
  NOTE_D,
  NOTE_E,
  NOTE_F,
  NOTE_G,
  NOTE_A,
  NOTE_B,
  NOTE_C
};

// output_frequency = sample_rate / (2^17) * frequency_word
// freq = 0.37252903 * fword
/*
// C#,D,D#,x,E,F,F#,x,G,G#,A,x,A#,B,C
$note = array( // values for when YM2151 is at 3.5MHz
	277.18, 293.66, 311.13, 0,  // c# d  d#  x
	329.63, 349.23, 369.99, 0,  // e  f  f#  x
	392.00, 415.30, 440.00, 0,  // g  g# a   x
	466.16, 493.88, 523.25, 0); // a# b  c   x
*/

int psgfreq[7] = { // D E F G A B C
  788<<3, 885<<3, 937<<3, 1052<<3, 1181<<3, 1326<<3, 1405<<3
};

char ymfreq[7] = {
  0x01, 0x04, 0x05, 0x08, 0x0a, 0x0d, 0x0e
};

void ymwrite(char r, char v) {
  while(YM2151.data & 0x80) {}
  YM2151.reg = r;
  __asm__ ("NOP");
  YM2151.data = v;
}

void psgwrite(char r, char v) {
  VERA.address = PSGBASE + r;
  VERA.address_hi = 1 | VERA_INC_1;
  VERA.data0 = v;
}

void patchYM(char chan) {
  char reg, p;
  ymwrite(0x20+chan,ympatch[0]);
  p=0;
  for (reg=0x38+chan ; reg >= 0x38 ; reg += 8)
    ymwrite(reg,ympatch[++p]);
}

void initPSG(char chan) {
  chan = chan*4;
  psgwrite(chan,0);
  psgwrite(chan+1,0);
  psgwrite(chan+2,0);
  psgwrite(chan+3,0x3F);
}

void silence() {
  char reg;
  for(reg=0xE0 ; reg >= 0xE0 ; reg += 8)
    ymwrite(reg,0x0F);
  for(reg=0;reg<8;++reg)
    ymwrite(8,reg);
  for(reg=2 ; reg<64 ; reg+=4)
    psgwrite(reg,0);
}

void play(char chip, char oct, char note) {
  int f;
  printf("playing note %d oct %d on chip",note,oct);
  if (chip & 1) {
    printf(" YM");
    ymwrite(8,0);
    ymwrite(0x28,ymfreq[note] | (oct<<4));
    ymwrite(8,0x78);
  }
  if (chip & 2) {
    printf (" PSG");
    f = psgfreq[note] >> (7-oct);
    psgwrite(0,f & 0xff);
    psgwrite(1,f >> 8);
    psgwrite(2,0xff);
  }
  printf("\n");
}

void main() {
  char key, y;

  char note = NOTE_A;
  char oct = 4;
  char chip = 1; // 1 = VERA, 2 = YM, 3 = both

  for(key=0 ; key<8 ; key++)
    patchYM(key);

  for(key=0 ; key<16 ; key++)
    initPSG(key);

  printf("1234567: Set Octave\n");
  printf("WERTYUI: Play DEFGABC\n");
  printf("SPACE = silence\n");
  printf("ENTER = toggle VERA/YM/BOTH\n");
  printf("ESC = quit\n\n");

  y = wherey();
  while(kbhit()) cgetc();
  key=0;
  while(key != CH_STOP) {
    vsync();
    if (kbhit()) {
      key = cgetc();
      if (key>='0' && key<'8') {
        oct = key-'0';
        play(chip,oct,note);
      }
      switch(key) {
      case CH_STOP:
      case ' ':
        silence();
        break;
      case CH_ENTER:
        silence();
        if (++chip >= 4) chip=1;
        play(chip,oct,note);
        break;
      case 'w':
        note = NOTE_D;
        play(chip,oct,note);
        break;
      case 'e':
        note = NOTE_E;
        play(chip,oct,note);
        break;
      case 'r':
        note = NOTE_F;
        play(chip,oct,note);
        break;
      case 't':
        note = NOTE_G;
        play(chip,oct,note);
        break;
      case 'y':
        note = NOTE_A;
        play(chip,oct,note);
        break;
      case 'u':
        note = NOTE_B;
        play(chip,oct,note);
        break;
      case 'i':
        note = NOTE_C;
        play(chip,oct,note);
        break;
      }
    }
  }
  silence();
}
