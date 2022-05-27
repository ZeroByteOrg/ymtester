.include "x16.inc"
.include "via.inc"

; NOTE - gonna delete this later and do this in C instead.

.importzp datapointer

.import ymwrite

.export play_scale


.bss
delay:      .res 1
lastjiffy:  .res 1

.code
done:
  rts

.proc play_scale: near
  lda #<SCALE
  sta datapointer
  lda #>SCALE
  sta datapointer+1
get_delay:
  lda (datapointer)
  tax
  cpx #0
  beq play_note
  cpx #$ff
  beq done
tick:
  jsr RDTIM
  sta lastjiffy
keep_waiting:
  jsr RDTIM
  cmp lastjiffy
  beq keep_waiting
  dex
  bne tick

play_note:
  ldy #1
  lda (datapointer),y ; get reg
  tax
  iny
  lda (datapointer),y ; get val
  tay
  ; advance data pointer to next 3-byte tuple
  lda #3
  clc
  adc datapointer
  sta datapointer
  lda datapointer+1
  adc #0
  sta datapointer+1
  jsr ymwrite
  bcs



.rodata
; YM data tables - format = delay, reg, val, ...  delay FF = done
SCALE:
  .byte 00,$08,$00,  00,$28+0,$2e,  00,$08,$78+0
  .byte 30,$08,$01,  00,$28+1,$31,  00,$08,$78+1
  .byte 30,$08,$02,  00,$28+2,$34,  00,$08,$78+2
  .byte 30,$08,$00
  .byte 00,$08,$03,  00,$28+3,$35,  00,$08,$78+3
  .byte 30,$08,$01
  .byte 00,$08,$04,  00,$28+4,$38,  00,$08,$78+4
  .byte 30,$08,$02
  .byte 00,$08,$05,  00,$28+5,$3a,  00,$08,$78+5
  .byte 30,$08,$03
  .byte 00,$08,$06,  00,$28+6,$3d,  00,$08,$78+6
  .byte 30,$08,$04
  .byte 00,$08,$07,  00,$28+7,$3e,  00,$08,$78+7
  .byte 30,$08,$05
  .byte 30,$08,$06
  .byte 30,$08,$07,  $ff
