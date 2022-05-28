.include "x16.inc"

.export _vsync

.bss
lastjiffy:  .res 1

.code
.proc _vsync: near
  jsr RDTIM
  sta lastjiffy
keep_waiting:
  jsr RDTIM
  cmp lastjiffy
  beq keep_waiting
  rts
.endproc
