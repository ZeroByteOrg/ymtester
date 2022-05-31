; This test should trigger the YM busy state and observe the busy flag
; as going high, and then low.

; Counter behavior:
; Test fail = never saw busy, or busy never cleared once observed.
;
; Does not use the selected write method - writes directly to YM and only
; bumps the ym_nobusy / ym_yesbusy counters upon a successful write.
;

.import nobusy
.import _ym_nobusy
.import _ym_yesbusy
.import _ym_timeouts
.import _ym_badstatus

.export _stress_ymbusy

.code
.proc _stress_ymbusy: near
  lda #1
  sta nobusy
  lda #$7F ; bad YM_STATUS mask - all of these bits should be clear
  ldx #0
  ldy #128 ; busy timeout

; do an initial safe wait of 256 NOP loops in order to ensure YM is not busy
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
  bpl busy_detected
  dey
  bne require_busy
  ; busy never seen
  inc _ym_nobusy
  bne return_failure
  inc _ym_nobusy+1
  bra return_failure

busy_detected:
  stz nobusy

dirty:
  inc _ym_badstatus
  bne return_failure
  inc _ym_badstatus+1
  bra return_failure

.endproc

return_failure:
  lda #1
  bra success2
return_success:
  lda #0
success2:
  ldx #0
  rts
