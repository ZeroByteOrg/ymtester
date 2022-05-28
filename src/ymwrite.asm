.include "x16.inc"
.include "via.inc"

; 3 routines to perform YM2151 writes.
; All 3 use the following API:
; .X = YM register
; .Y = YM value
; Returns: .A = 0 if successful, 1 if failed
;          Z flag is also set if successful, clear if failed
;
; _ymwrite is the routine exposed to C. It converts the C arguments into
; the format above and jumps to the currently-selected write method.
;
; _ymwrite_setmethod is the C call to select between the 3 methods.
;


  .import _ym_timeouts  ; uint16_t
  .import _ym_nobusy    ; uint16_t
  .import _ym_badstatus ; uint16_t

  .import popa
  .export _ymwrite
  .export _ymwrite_setmethod
  .export ymwrite
  .export ymwrite_setmethod

.bss
nobusy:         .res 1
busy_timeout:   .res 1

;--------------------------------------------------------------------

.rodata
WR_METHOD_LO:
  .byte <ymwrite_nops, <ymwrite_viat1, <ymwrite_busyflag
WR_METHOD_HI:
  .byte >ymwrite_nops, >ymwrite_viat1, >ymwrite_busyflag

NUM_METHODS = 3

;--------------------------------------------------------------------

_ymwrite_setmethod:
.proc ymwrite_setmethod: near
  ; .A = chosen method #
  ; returns A=1 if error, A=0 if successful
  cmp #NUM_METHODS
  bcs exit ; with 1 in the carry flag
  tax
  sei
  lda WR_METHOD_LO,x
  sta ymwrite_vector
  lda WR_METHOD_HI,x
  sta ymwrite_vector+1
  cli
exit:
  lda #0
  rol ; .A = carry flag for return value to C program
  rts
.endproc

;--------------------------------------------------------------------

.code
_ymwrite:
  ; from C __fastcall__: .A = val, top-of-stack = reg
  ; transfer them to .X = reg .Y = val for asm routine call.
  ; jumps to the currently-selected write method
  tay
  jsr popa
  tax
  jmp ymwrite_nops
  ymwrite_vector := (*-2)

  ; expects .X = reg and .Y = val as the three asm routines use.
ymwrite:
  jmp (ymwrite_vector)

;--------------------------------------------------------------------

.proc ymwrite_nops: near
  lda #64
  nop
  dec
  bne ymwrite_nops
  stx YM_reg
  nop
  nop
  nop
  sty YM_data
  lda #0 ; return success
  rts
.endproc

;--------------------------------------------------------------------

.proc ymwrite_viat1: near
  lda #0
checkvia:
  bit VIA2_ifr
	bvs	go
  dec
  bne checkvia
  bra timeout

go:
  stx	YM_reg
	nop
	nop
	nop
	sty	YM_data
	stz VIA2_t1ch
  lda #0
	rts
.endproc

timeout:
  ; increment the timeouts counter
  inc _ym_timeouts
  bne :+
  inc _ym_timeouts+1
: lda #1 ; return failure
  rts

;--------------------------------------------------------------------

.proc ymwrite_busyflag: near
  lda #1
  sta nobusy
  lda #64 ; overly-generous timeout for YM2151 busy flag to become clear.
  sta busy_timeout

; this "bad read" check assumes that the timers are not running and that
; the overflow flags are clear.
  lda #$7f  ; mask to check that the lower bits are all zero
checkbusy:
  bit YM_data
  bne bad_ym_status ; bits other than the busy flag are set. (bad read)
  bpl go
  stz nobusy
  dec busy_timeout
  bne checkbusy
  bra timeout

go:
  stx YM_reg
  nop
  nop
  nop
  sty YM_data
  lda nobusy ; 1 = no busy flag observed, 0 = busy flag observed
  ; add nobusy to the _ym_nobusy counter
  clc
  adc _ym_nobusy
  sta _ym_nobusy
  lda #0
  adc _ym_nobusy+1
  sta _ym_nobusy+1
  lda #0 ; return success
  rts
.endproc

bad_ym_status:
  inc _ym_badstatus
  bne :+
  inc _ym_badstatus+1
: lda #1  ; return failure
  rts
