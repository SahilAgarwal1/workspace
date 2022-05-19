; program for lab quiz
;Sahil Agarwal

	.global _c_int00


vect1_addr .set 0x8022
vect2_addr .set 0x8122

WD_Regs .set 0x7000
WDCR .set 0x29
WDRC_data .set 0x68

GPIO_CTRL_REGS .set 0x7C00
GPAMUX1 .set 0x6
GPAMUX2 .set 0x8
GPADIR .set 0xA

GPAMUX1_data .set 0x00
GPAMUX2_data .set 0x00

GPADIR_data_word1 .set 0x00FF

GPIO_DATA_REGS .set 0x7F00
GPADAT .set 0x0
GPASET .set 0x2
GPACLR .set 0x4

GPAPUD .set 0xC

GPAPUD_data_word1 .set ~(1100111100000000b)


.data
counter .word 0x0

vect_min .usect ".ebss", 1 ; reserve 1 word for min
vect_max .usect ".ebss", 1 ; reserve 1 word for max
vect_sum .usect ".ebss", 2 ; reserve 1 word for the sum


	.text

_c_int00:

	; turn off watchdog register
	SETC OBJMODE ; C28X mode
	EALLOW ; enable access to write protected registers
	MOVZ DP, #WD_Regs >> 6
	MOV @(WDCR + WD_Regs), #WDRC_data ; turn off watchdog driver

	; setting up GPIO
	MOVZ DP, #GPIO_CTRL_REGS >> 6
	MOV @(GPIO_CTRL_REGS + GPAMUX1), #GPAMUX1_data ; set mux
	MOV @(GPIO_CTRL_REGS + GPAMUX2), #GPAMUX2_data	; set mux
	MOV @(GPIO_CTRL_REGS + GPAPUD), #GPAPUD_data_word1 ; set the pull up resistor on GPIO3
	MOV @(GPIO_CTRL_REGS + GPADIR), #GPADIR_data_word1 ; set the direction

LOOP:
	MOVZ DP, #GPIO_DATA_REGS >> 6
	TBIT @(GPIO_DATA_REGS + GPADAT), #14
	B MULT, NTC

	; getting min and max values
	MOVL XAR0, #counter
	MOVL XAR1, #vect1_addr
	MOVL XAR2, #vect2_addr
	; set up counter
	MOV AH, #5
	MOV *AR0, AH
	;Load First Value into AH
	MOV AH, *XAR1
GET_MIN:

	MIN AH, *XAR1++
	MIN AH, *XAR2++

	DEC *XAR0;
	B GET_MIN, NEQ
	MOV AR3, #vect_min
	MOV *XAR3, AH ; store AH in vect_min

	; getting min and max values
	MOVL XAR0, #counter
	MOVL XAR1, #vect1_addr
	MOVL XAR2, #vect2_addr
	; set up counter
	MOV AH, #4
	MOV *AR0, AH

	;Load First Value into AH
	MOV AH, *XAR1++
GET_MAX:

	MAX AH, *XAR1++
	MAX AH, *XAR2++

	DEC *XAR0;
	B GET_MAX, NEQ
	MOV AR3, #vect_max
	MOV *XAR3, AH ; store AH in vect_max
	B LOOP, UNC
MULT:
	MOVL XAR0, #counter
	MOVL XAR1, #vect1_addr
	MOVL XAR2, #vect2_addr

	; set up counter
	MOV AH, #5
	MOV *AR0, AH
	MOV AH, #0
	MOV AL, #0
MULT_LOOP:
	MOVA T, *XAR1++ ; MOV Vector 1 first value into T reg and add product to accumulator
	MPY P,T,*XAR2++
	DEC *AR0
	B MULT_LOOP, NEQ
	MOVL XAR3, #vect_sum
	MOVL *XAR3, ACC

	B LOOP, UNC




