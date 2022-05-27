.include "x16.inc"
.include "via.inc"

.import ymwrite

.export play_scale

.zeropage
data: .res 2

.bss
delay:      .res 1
lastjiffy:  .res 1

.code
done:
  rts
play_scale:
  stz delay
  lda #<scale
  sta data
  lda #>scale
  sta data+1
  bra tick
wait_frame:
  lda delay
  beq next_note
  cmp #$ff
  beq done
  dec
  sta delay
  jsr RDTIM
  sta lastjiffy
keep_waiting:
  jsr RDTIM
  cmp lastjiffy
  beq keep_waiting
tick:
  lda delay
  beq next_note
  cmp #$ff
  beq done
  dec
  sta delay


.rodata
; YM data tables - format = delay, reg, val, ...  delay FF = done
:scale
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
