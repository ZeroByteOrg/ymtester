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


  .export _count_fail_busy  ; uint16_t
  .export _count_fail_nobusy    ; uint16_t
  .export _count_fail_badread ; uint16_t
  .export _count_ok_busy   ; uint16_t

  .import popa
  .export _ymwrite
  .export _ymwrite_setmethod
  .export ymwrite
  .export ymwrite_setmethod

  ; allow other assembly routines to use these tmp locations and routines
  .export nobusy
  .export wait_timer
  .export success_busy
  .export fail_busy
  .export fail_nobusy
  .export fail_dirty
  .export return_success

.bss
nobusy:         .res 1
wait_timer:   .res 1

.data
_count_fail_busy:     .word 0 ; failures: BUSY flag never cleared
_count_fail_nobusy:   .word 0 ; failures: BUSY flag was never asserted
_count_fail_badread:  .word 0 ; failures: STATUS byte had 1 in any bits 0-6.
_count_ok_busy:       .word 0 ; Success count whe BUSY flag was initially set

; Note that BUSY means bit7 of YM status byte, or the timeer status bit in
; VIA if using the VIA as a stand-in busy timer for the YM. (not yet implemented)

;--------------------------------------------------------------------

.rodata
WR_METHOD_LO:
  .byte <ymwrite_nops, <ymwrite_viat1, <ymwrite_busyflag
WR_METHOD_HI:
  .byte >ymwrite_nops, >ymwrite_viat1, >ymwrite_busyflag

NUM_METHODS = 3

;--------------------------------------------------------------------

.code
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
ymwrite: ; expects .X = reg and .Y = val as the three asm routines use.
  jmp ymwrite_nops
  ymwrite_vector := (*-2)

;--------------------------------------------------------------------

.code
.proc ymwrite_nops: near
  lda #64
do_nop:
  nop
  dec
  bne do_nop
  stx YM_reg
  nop
  nop
  nop
  sty YM_data
  jmp return_success
.endproc

;--------------------------------------------------------------------

.code
.proc ymwrite_viat1: near
  ; TODO: implement "nobusy" check/count for this write method
  lda #0
checkvia:
  bit VIA2_ifr
	bvs	go
  dec
  bne checkvia
  jmp fail_busy

go:
  stx	YM_reg
	nop
	nop
	nop
	sty	YM_data
	stz VIA2_t1ch
  jmp return_success
.endproc

;--------------------------------------------------------------------

.code
.proc ymwrite_busyflag: near
  lda #1
  sta nobusy
  lda #64 ; overly-generous timeout for YM2151 busy flag to become clear.
  sta wait_timer

  ; this "bad read" check assumes that the timers are not running and that
  ; the overflow flags are clear.
  lda #$7f  ; mask to check that the lower bits are all zero
checkbusy:
  bit YM_data
  bne fail_dirty ; bits other than the busy flag are set. (bad read)
  bpl go
  stz nobusy
  dec wait_timer
  bne checkbusy
  jmp fail_busy

go:
  stx YM_reg
  nop
  nop
  nop
  sty YM_data
  lda nobusy ; 1 = no busy flag observed, 0 = busy flag observed
  bne success_busy ; we want to count times no busy was observed
  bra return_success ; inc the ok_busy counter if busy was encountered
.endproc

;--------------------------------------------------------------------

.code
success_busy:
  inc _count_ok_busy
  bne return_success
  inc _count_ok_busy+1
  bra return_success

fail_dirty:
  inc _count_fail_badread
  bne return_fail
  inc _count_fail_badread+1
  bra return_fail

fail_busy:
  inc _count_fail_busy
  bne return_fail
  inc _count_fail_busy+1
  bra return_fail

fail_nobusy:
  inc _count_fail_nobusy
  bne return_fail
  inc _count_fail_nobusy+1
  bra return_fail

return_fail:
  lda #1
  bra :+
return_success:
  lda #0
: ldx #0
  rts
