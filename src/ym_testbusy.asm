.include "x16.inc"

; This test should trigger the YM busy state and observe the busy flag
; as going high, and then low.

; Counter behavior:
; Test fail = never saw busy, or busy never cleared once observed.
;
; Does not use the selected write method - writes directly to YM and only
; bumps the ym_ok_busy counter upon a successful write where not instantly busy
;

; import variable space from ymwrite.asm:
.import _count_fail_busy
.import _count_fail_nobusy
.import _count_fail_badread
.import _count_ok_busy
.import nobusy ; tmp "boolean" for "not immediately busy but eventually"

; import utility code snippets from ymwrite.asm
.import success_busy
.import fail_busy
.import fail_nobusy
.import fail_dirty
.import return_success

; export this routine to the rest of the project:
; args = none
; return = .AX  (0 = success, 1 = failure)
.export _ym_testbusy

.code
.proc _ym_testbusy: near
  lda #1
  sta nobusy
  lda #$7F ; bad YM_STATUS mask - all of these bits should be clear
  ldx #0
  ldy #128 ; busy timeout

; do an initial safe wait of 256 busy loops in order to ensure YM is not busy
safe_delay:
  dex
  bne safe_delay

  ; cause YM to become busy with a write to reg 0 = 0.
  stz YM_reg
  nop
  nop
  nop
  stz YM_data

require_busy:
  bit YM_data
  bne dirty
  bmi busy_detected
  stz nobusy
  dey
  bne require_busy
  jmp fail_nobusy

busy_detected:
  ldy #128 ; busy timeout
still_busy:
  bit YM_data
  bne dirty
  bpl success
  dey
  bne still_busy
  jmp fail_busy

success:
  ldx nobusy
  bne done
  inc _count_ok_busy
  bne done
  inc _count_ok_busy+1
done:
  jmp return_success

dirty:
  jmp fail_dirty

.endproc
