; LAB 3 QUIZ

	.global _c_int00

WD_Regs 	.set 0x7000
WDCR 		.set 0x29
WDRC_data 	.set 0x68

Input		.set 5.0


	.data
Output		.usect ".ebss", 2;


; PROGRAM START

	.text

_c_int00:
	EALLOW ; write to protected registers

	; turn off the watchdog timer
	MOVZ DP, #WD_Regs >> 6
	MOV @(WDCR + WD_Regs), #WDRC_data ; turn off watchdog driver

	; initialize the stack pointer
	MOV SP, #0x400

	;START FPU CALCULATIONS
	MOVF32 R0H, #Input ; MOV Input into R1H
	MOVF32 R1H, #7.14
	MPYF32 R0H, R1H, R0H
	nop ; wait for result to be done
	MOVF32 R1H, #3.13
	ADDF32 R0H, R1H, R0H ; add the terms together
	nop ; wait for result to be done
	MOVL XAR0, #Output
	MOV32 *XAR0, R0H

DONE:
	B DONE, UNC




