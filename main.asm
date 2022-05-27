.include "x16.inc"
.include "via.inc"


; NOTE - gonna delete this and use C for the UI portion instead....

.segment "ONCE"

.segment "STARTUP"
startup:

.code
helloworld:


.rodata

HELLOWORLD: .asciiz "hello world!"
