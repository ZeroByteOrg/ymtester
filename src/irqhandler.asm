.include "x16.inc"
.include "via.inc"

.import _ym_status_byte
.import ymwrite_nops    ; safe delay with NOPS for ym write

.export _irqhandler
.export _kernal
.export _irq_waiting ; a semaphore - the handler STZs it when run
.export _irq_ym_status

.code
go_kernal:
  jmp $FFFF
  _kernal := (*-2)

_irq_waiting:
  .byte 0
_irq_ym_status:
  .byte 0

.proc _irqhandler: near
  ; check VERA and VIA2 for IRQs first.
  ; assume YM2151 may not be readable. If none of those
  ; chips has an IRQ set, then it must be YM2151
  lda VERA_ien
  and #$0f
  and VERA_isr
  bne go_kernal

  ; skip VIA 1 because it triggers NMI, not IRQ
  ;bit VIA1_ifr
  ;bmi go_kernal

  bit VIA2_ifr
  bmi go_kernal

  stz _irq_waiting
  lda YM_data
  sta _irq_ym_status
exit_irq:
  ; ACK to YM using safe write (nops.. in case it's busy)
  ldx #$14
  ldy #$30
  jsr ymwrite_nops
  ply
  plx
  pla
  rti
.endproc
