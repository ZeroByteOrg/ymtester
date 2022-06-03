.include "x16.inc"

.define NUM_CHECKS 32
.define DATA $A000 ; use HiRam

; these will be replaced with the right char codes later.
BYTE_GARBAGE = 'x' ;$D6
BYTE_HI = '^'
BYTE_LO = '.'
BYTE_RI = '^'
BYTE_FA = '.'

.import fail_busy
.import fail_nobusy
.import fail_dirty
.import fail_flap
.import success_busy
.import return_success

.import _count_fail_badread

.export _ym_probe_busy

.code

lastbyte: .byte 0
errors:   .byte 0
n_rise:   .byte 0
n_fall:   .byte 0
lastedge: .byte 0

.proc _ym_probe_busy: near
  stz RAM_BANK
  inc RAM_BANK
  ; make sure the YM is ready for writing.
  ldx #0
long_busy_wait:
  dex
  bne long_busy_wait
  ; force a busy condition by writing 0 into unused reg 0.
  stz YM_reg
  nop
  nop
  nop
  stz YM_data
  ; poll the busy flag NUM_CHECKS times as rapidly as possible
.repeat NUM_CHECKS , I
  lda YM_data
  sta DATA+I
.endrep

evaluate_data:
  lda #1
  sta errors ; initialize to 1. Any error will zero this out.
  ; ldx #0 unneccessary - already 0 from end of long_busy_wait
  ldy #$FF ; -1 so pre-increment makes Y start at 0.
  stz n_rise
  stz n_fall
nextbyte:
  iny
  cpy #NUM_CHECKS
  beq finish
  lda DATA,y
  bit #$7f ; check for dirty bits 0-6
  beq check_edges
  ; byte is dirty
  stz errors
  inc _count_fail_badread
  bne :+
  inc _count_fail_badread+1
: lda #BYTE_GARBAGE
  sta DATA,y
  bra nextbyte

check_edges:
  cpx #0 ; have we encountered a non-garbage byte yet?
  bne check_rising
  inx ; not yet - so bump X to indicate we've hit the first good byte read.
  sta lastbyte  ; and save it to lastbyte
  bra check_low ; then check whether it's a low or high byte for the display
check_rising:
  ora lastbyte
  eor lastbyte
  beq check_falling
  sta lastbyte  ; $80 is the result of a rising edge detect, which is OK to save
  lda #1
  sta lastedge
  inc n_rise
  lda #BYTE_RI
  sta DATA,y
  bra nextbyte
check_falling:
  lda DATA,y
  eor lastbyte
  and lastbyte
  beq check_low
  stz lastbyte
  stz lastedge
  inc n_fall
  lda #BYTE_FA
  sta DATA,y
  bra nextbyte
check_low:
  lda DATA,y
  sta lastbyte
  cmp #0
  bne set_hi
  lda #BYTE_LO
  sta DATA,y
  bra nextbyte
set_hi:
  lda #BYTE_HI
  sta DATA,y
  bra nextbyte

finish:
  lda errors ; if errors=0 then at least one dirty read occurred.
  bne :+
  jmp fail_dirty
: clc
  lda n_rise
  adc n_fall
  tax
  cmp #3 ; check whether 3 or more edges were detected. If so - flapping signal
  bcc :+
  jmp fail_flap
: cmp #0
  bne check_final_edge
  ; no edges detected. Determine never busy vs always busy
  dey
  lda DATA,y
  cmp #BYTE_HI
  beq :+
  jmp fail_nobusy
: jmp fail_busy
check_final_edge:
  lda lastedge
  beq ok_go ; here it goes, here it goes, here it goes, here it goes again!
  jmp fail_flap
ok_go:
  ; last question: 1 or 2 edges (to determine whether to bump OK_BUSY)
  txa
  dex
  bne :+
  jmp return_success
: jmp success_busy
.endproc
  ; wow - what a mouthfull THIS routine turned out to be!!!
