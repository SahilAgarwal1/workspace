; File to Convert Vector of Fahrenhiet to Celsius

	.global _c_int00

WD_Regs 	.set 0x7000
WDCR 		.set 0x29
WDRC_data 	.set 0x68

T_count		.set 	7 ; num of temperatures

far_vect	.set	0x8111
cel_vect	.set	0x8311
	.data
;far_vect	.float 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0 ; vector of farhenhiet values
;			.float 100.0, 110.0, 120.0, 130.0, 140.0, 150.0
;			.float 160.0, 170.0, 180.0, 190.0, 200.0, 210.0

;cel_vect 	.usect ".ebss", 18*2 ; reserve 18*2 words for 18 floats
counter		.usect ".ebss", 1;

;******** program start********************

	.text
_c_int00:
	EALLOW ; write to protected registers

	; turn off the watchdog timer
	MOVZ DP, #WD_Regs >> 6
	MOV @(WDCR + WD_Regs), #WDRC_data ; turn off watchdog driver

	; initialize the stack pointer
	MOV SP, #0x400

	; initialize counter
	MOVL  XAR0, #counter ; AR0 is pointer to counter in mem
	MOVL XAR1, #T_count
	MOVL *XAR0, XAR1; move value of T into counter

	; initialize pointer to FAR and CEL
	MOVL XAR1, #far_vect
	MOVL XAR2, #cel_vect

LOOP:
	MOVL ACC, *XAR1++ ; Load ACC with First Temp and iterate pointer
	PUSH ACC
	LC F_to_C
	POP ACC
	MOVL *XAR2++, ACC
	DEC *XAR0
	B LOOP, NEQ
DONE:
	B DONE, UNC






F_to_C:

	POP XAR7 ; save adress from stack
	POP ACC ; get the temp from stack and put into ACC

	;start FPU instructions
	MOV32 R0H, @ACC ; move value from accumulator to R0H

	NOP	; 4 NOPS to alight R0H
	NOP
	NOP
	NOP

	MOVF32 R1H, #32.0 ; MOV 32 into R1H
	SUBF32 R0H, R0H, R1H ; SUBtract 32 From R0H

	NOP ; Wait for R0H to be valid

	MPYF32 R0H, #5.0, R0H
	MOVF32 R1H, #9.0 ; load R1H with 9 while also waiting for R0H to be valid

	EINVF32 R1H, R1H ; find inverse of 9
	NOP ; wait for R1H to be valid

	MPYF32 R0H, R1H, R0H
	NOP ; wait for R0H to be valid
	NOP ; align with MOV32
	NOP ; delay again

	MOV32 @ACC, R0H ; MOV r)H to ACC
	PUSH ACC ; PUSH ACC
	PUSH XAR7 ; PUSH RETURN ADDRESS
	LRET ; RETURN FROM SUBROUTINE

















