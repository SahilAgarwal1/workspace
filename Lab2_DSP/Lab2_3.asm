;Program to Get inputs and put them on output LEDS

	.global _c_int00

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
GPADIR_data_word2 .set 0x0000


GPIO_DATA_REGS .set 0x7F00
GPADAT .set 0x0
GPASET .set 0x2
GPACLR .set 0x4

GPAPUD .set 0xC

GPAPUD_data_word1 .set 0x30FF
GPAPUD_data_word2 .set 0xFFFE

.data

temp .word 0x00



	.text

_c_int00:


	SETC OBJMODE ; C28X mode
	EALLOW ; enable access to write protected registers
	MOVZ DP, #WD_Regs >> 6
	MOV @(WDCR + WD_Regs), #WDRC_data ; turn off watchdog driver

	;setting up GPIO
	MOVZ DP, #GPIO_CTRL_REGS >> 6
	MOV @(GPIO_CTRL_REGS + GPAMUX1), #GPAMUX1_data ; set mux
	MOV @(GPIO_CTRL_REGS + GPAMUX2), #GPAMUX2_data	; set mux
	MOV @(GPIO_CTRL_REGS + GPAPUD), #GPAPUD_data_word1 ; set pull up
	MOV @(GPIO_CTRL_REGS + GPAPUD + 1), #GPAPUD_data_word2 ; set pull up
	MOV @(GPIO_CTRL_REGS + GPADIR), #GPADIR_data_word1 ; set direction
	MOV @(GPIO_CTRL_REGS + GPADIR + 1), #GPADIR_data_word2 ; set direction



LOOP:
	MOVZ DP, #GPIO_DATA_REGS >> 6
	MOV AL, @(GPIO_DATA_REGS + GPADAT) ; grab data from the IO port
	MOV T, #8 ; load T with 8
	LSR AL, T ; shift AL right by 8
	MOV @(GPIO_DATA_REGS + GPADAT), AL
	B LOOP, UNC
DONE:
	B DONE, UNC









