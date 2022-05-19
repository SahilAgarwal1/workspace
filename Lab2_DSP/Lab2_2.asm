;Program to instantiate and Send out LED data

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

GPADIR_data .set 0x7F


GPIO_DATA_REGS .set 0x7F00
GPADAT .set 0x0


	.text

_c_int00:


	SETC OBJMODE ; C28X mode
	EALLOW ; enable access to write protected registers
	MOVZ DP, #WD_Regs >> 6
	MOV @(WDCR + WD_Regs), #WDRC_data ; turn off watchdog driver

	;setting up GPIO
	MOVZ DP, #GPIO_CTRL_REGS >> 6
	MOV @(GPIO_CTRL_REGS + GPAMUX1), #GPAMUX1_data
	MOV @(GPIO_CTRL_REGS + GPAMUX2), #GPAMUX2_data
	MOV @(GPIO_CTRL_REGS + GPADIR), #GPADIR_data
	MOVZ DP, #GPIO_DATA_REGS >> 6
	MOV @(GPIO_DATA_REGS + GPADAT), #0x00

DONE:
	B DONE, UNC









