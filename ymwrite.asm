.include "x16.inc"
.include "via.inc"

; 3 routines to perform YM2151 writes.
; All 3 use the following API:
; .X = YM register
; .Y = YM value
; .A gets clobbered
; Returns: C set = write failure (any reason)

.import num_timeouts
.import num_nobusy

.export ymwrite
.export ymwrite_vector
.export ymwrite_nops
.export ymwrite_viat1
.export ymwrite_busyflag

.code
ymwrite:
  ; jumps to the currently-selected write method
  jmp ymwrite_nops
  ymwrite_vector := (*-2)

ymwrite_nops:
  lda #64
  nop
  dec
  bne ymwrite_nops
  stx YM_reg
  nop
  nop
  nop
  sty YM_data
  clc
  rts

ymwrite_viat1:
  lda #0
: bit	VIA2_ifr
	bvs	:+
  dec
  bne :-
  bra timeout
: stx	YM_reg
	nop
	nop
	nop
	sty	YM_data
	stz VIA2_t1ch
  clc
	rts

timeout:
  inc num_timeouts
  bne :+
  inc num_timeouts+1
: sec
  rts

nobusy:  .byte 1

ymwrite_busyflag:
  lda #1
  sta nobusy
  lda #0
: bit YM_data
  bpl :+
  stz nobusy
  dec
  bne :-
  bra timeout
: stx YM_reg
  nop
  nop
  nop
  sty YM_data
  lda nobusy
  clc
  adc num_nobusy
  sta num_nobusy
  lda #0
  adc num_nobusy+1
  sta num_nobusy+1
  clc
  rts
