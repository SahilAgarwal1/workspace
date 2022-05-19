; Sahil Agarwal
; asm file to iterate through an array and check number values


; assembler directive to allow linker to know where to start

	.global _c_int00

; setting program constants

vect_addr	.set	0x8222
length_N	.set	12

; data for the program to go through
.data

counter .word 0h
array .word 1,2,3,4,5,12,13,-1, -2, -3, -4, -12



X_sum .usect ".ebss", 1 ; reserve 1 word in ebss for X_sum
Y_sum .usect ".ebss", 1 ; reserve 1 word in ebss for Y_sum


.text

_c_int00:

	; initialize the registers

	MOVL XAR0, #counter ; XAR0 loaded with counter pointer
	MOVL XAR1, #vect_addr ; XAR1 loaded with vector pointer
	MOVL XAR2, #X_sum ; X sum pointer
	MOVL XAR3, #Y_sum ; Y sum pointer

	; initialize the counter
	MOV AH, #length_N		; load the length of the array
	MOV *AR0, AH 	; store the length of the array into the counter variable

	;load value from vector into the AH and iterate pointer
LOOP:
	MOV AH, *XAR1++
	CMPB AH, #04h ; compare with A
	B LESS, LT ; if LESS than BRanch to less, otherwise it must be greater than or = so continue
	; continue with program if more so sum Y
	ADD *XAR3, AH ; add AH to Y_sum
DECR:
	DEC *XAR0 ; Decrement XAR0
	B DONE, EQ
	B LOOP, UNC
LESS:
	ADD *XAR2, AH
	B DECR, UNC
DONE:
	B DONE, UNC




















