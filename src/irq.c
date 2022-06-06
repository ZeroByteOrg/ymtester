#include "irq.h"
#include <stdint.h>

#define irq_vector (*(uint16_t*)0x0314)

extern void __fastcall__ irqhandler();
// irqhandler checks for IRQs from all other sources
// and then if none are set, assumes the YM2151 was the source.
// It sets irq_waiting=0 to signal that a YM2151 IRQ was
// detected. It then reads the status byte, saves it as irq_ym_status, and
// attempts to ACK the YM2151 using the safe write method.

extern uint16_t kernal; // holds the previous value of the $314 vector.


void irq_install() {
  // make sure the handler isn't already installed, to avoid
  // overwriting the original vector value in kernal
  if (irq_vector == (uint16_t)&irqhandler) return;
  kernal = irq_vector;
  asm("sei");
  irq_vector = (uint16_t)&irqhandler;
  asm("cli");
}

void irq_remove() {
  asm("sei");
  irq_vector = kernal;
  asm("cli");
}
