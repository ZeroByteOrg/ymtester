.export num_timeouts
.export num_fails
.export num_nobusy

.data
  num_timeouts:  .word 0
  num_fails:     .word 0
  num_nobusy     .word 0

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
