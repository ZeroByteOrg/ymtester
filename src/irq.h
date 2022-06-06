#ifndef _irq_h_
#define _irq_h_

extern unsigned char irq_waiting; // semaphore. the IRQ zeros it out when triggered.
extern unsigned char irq_ym_status; // the last value read from YM during IRQ

extern void irq_install();
extern void irq_remove();

#endif
