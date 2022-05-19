; File for bit banging to LCD

	.global _c_int00

WD_Regs 	.set 0x7000
WDCR 		.set 0x29
WDRC_data 	.set 0x68

GPIO_CTRL_REGS .set 0x7C00
GPBMUX1 .set 0x7C06
GPBMUX2 .set 0x7C08
GPBDIR .set 0x7C0A
GPBPUD .set 0x0C

GPIO_DATA_REGS .set 0x7F00
GPBDAT_offset .set 0x0

;For initial Data with Bit Banging
; SDA Line is HIGH, GPIO 32 is INPUT
; SCLK Line is HIGH, GPIO 33 is INPUT

GPBMUX1_data .set ~0x000F ; set everything as GPIO
GPBDIR_data	 .set ~0x0003 ; set all as inputs

	.data
test_bit_num .word 0x0
test_byte	.word 0x0

in_string		.word 'S', 'a', 'h', 'i', 'l', ' ', 'A', 'g', 'a', 'r', 'w', 'a', 'l', 0x00

counter_delay .usect ".ebss", 2
counter .usect ".ebss", 1
counter_string .usect ".ebss", 1

	.text

_c_int00:
	EALLOW

	; turn off the watchdog timer
	MOVZ DP, #WD_Regs >> 6
	MOV @(WDCR + WD_Regs), #WDRC_data ; turn off watchdog driver

	; initialize the stack pointer
	MOV SP, #0x400

	;initialize GPIO
	MOVZ DP, #GPBMUX1 >> 6
	AND @(GPBMUX1), #GPBMUX1_data ; set as GPIO
	AND @(GPBPUD), #GPBDIR_data ; set pullup
	AND @(GPBDIR), #GPBDIR_data	; set as INPUT

	LC I2C_DELAY


LOOP:
	LC INIT_LCD
	;LC I2C_WRITE_STRING
	B LOOP, UNC

DONE:
	B DONE, UNC



;***********SUBROUTINES************

;ROUTINE TO MAKE AN I2C DELAY
I2C_DELAY:
	PUSH XAR1;
	PUSH XAR0;
	MOVL XAR0, #counter_delay
	MOVL XAR1, #75000
	MOVL *XAR0, XAR1

LOOP_DELAY:
	NOP
	DEC *XAR0
	B LOOP_DELAY, NEQ
	POP XAR0;
	POP XAR1;
	LRET

;ROUTINE TO SET SCL PIN
I2C_SET_SCL:
	PUSH DP ; push the DATA Pointer
	MOVZ DP, #GPBDIR >> 6 ; set DP to GPIO CTRL REGS
	AND @(GPBDIR), #~0x0002 ; SET SCL to INPUT (HIGH)
	POP DP ; POP DP back
	LRET

I2C_SET_SDA:
	PUSH DP ; push the DATA Pointer
	MOVZ DP, #GPBDIR >> 6  ; set DP to GPIO CTRL REGS
	AND @(GPBDIR), #~0x0001 ; SET SCL to INPUT (HIGH)
	POP DP ; POP DP back
	LRET

I2C_CLR_SCL:
	PUSH DP ; push the DATA Pointer
	MOVZ DP, #GPBDIR >> 6  ; set DP to GPIO CTRL REGS
	OR @(GPBDIR), #0x02 ; SET SCL to OUTPUT (LOW)
	POP DP
	LRET

I2C_CLR_SDA:
	PUSH DP ; push the DATA Pointer
	MOVZ DP, #GPBDIR >> 6  ;set DP to GPIO CTRL REGS
	OR @(GPBDIR), #0x0001 ; SET SCL to OUTPUT (LOW)
	POP DP ; POP DP back
	LRET

I2C_START:
	LC I2C_SET_SCL ;SCL HIGH
	LC I2C_SET_SDA ; SDA HIGH
	LC I2C_DELAY ; DELAY
	LC I2C_CLR_SDA; SDA goes down first
	LC I2C_DELAY ; DELAY
	LC I2C_CLR_SCL ; SCL goes DOWN
	LC I2C_DELAY
	LRET

I2C_STOP:
	LC I2C_CLR_SDA
    LC I2C_DELAY
    LC I2C_SET_SCL
    LC I2C_DELAY
    LC I2C_SET_SDA
    LC I2C_DELAY
    LRET


I2C_WRITE_1: ; BIT TO WRITE STORED IN STACK modifies XAR7 and AL
	LC I2C_SET_SDA ; write out a 1
	LC I2C_DELAY
    LC I2C_SET_SCL
    LC I2C_DELAY
    LC I2C_CLR_SCL
    LC I2C_DELAY

    LRET

I2C_WRITE_0:
	LC I2C_CLR_SDA ; write out a 1
	LC I2C_DELAY
    LC I2C_SET_SCL
    LC I2C_DELAY
    LC I2C_CLR_SCL
    LC I2C_DELAY
    LRET


I2C_READ_BIT: ; leaves what is read in AL
	PUSH DP
	PUSH ACC
	LC I2C_SET_SDA
    LC I2C_DELAY
    LC I2C_SET_SCL
    LC I2C_DELAY
    MOVZ DP, #GPIO_DATA_REGS >> 6
    AND AL, @(GPBDAT_offset), #01
    MOVZ AR0, @AL

    POP ACC ; recover ACC
    POP DP	; recover DP


    POP XAR7 ; POP return address
    PUSH XAR0 ; push AR0
    PUSH XAR7 ; push return address
    LRET ; return to instruction

I2C_WRITE_BYTE:
	POP XAR7 ; POP THE RETURN ADDRESS
	POP XAR6 ; POP THE BYTE TO BE WRITTEN OUT into XAR6

	PUSH XAR1; save XAR1 and XAR0 incase needed
	PUSH XAR0;

	MOVL XAR0, #counter ; start a counter of 8
	MOVL XAR1, #8
	MOVL *XAR0, XAR1

WRITE_LOOP:
	DEC *XAR0 ; DEC the counter
	MOVAD T, *XAR0 ; MOV the counter value into T
	MOVL ACC, @XAR6 ; put the BYTE INTO the ACC
	LSRL ACC, T ; shift ACC over
	AND AL, #0x01
	B WRITE_ZERO, EQ
	LC I2C_WRITE_1
	CMP *XAR0, #0x0
	B WRITE_LOOP, NEQ
	B WRITE_DONE, UNC
WRITE_ZERO:
	LC I2C_WRITE_0
	CMP *XAR0, #0x0
	B WRITE_LOOP, NEQ
WRITE_DONE:
	POP XAR0
	POP XAR1
	PUSH XAR7
	LRET

WRITE_COMMAND_REG:
	LC I2C_START; #START THE I2C transmission
	;LC I2C_WRITE_0; RS = 0
	;LC I2C_WRITE_0; R/W = 0 WRITE
	POP XAR4 ; save the return address
	LC I2C_WRITE_BYTE
	LC I2C_STOP
	PUSH XAR4 ; put return address back
	LRET

INIT_LCD:
	; send 0x33
	MOVB XAR6, #0x33
	PUSH XAR6
	LC WRITE_COMMAND_REG
	; send 0x32
	MOVB XAR6, #0x32
	PUSH XAR6
	LC WRITE_COMMAND_REG
	; send 0x28
	MOVB XAR6, #0x28
	PUSH XAR6
	LC WRITE_COMMAND_REG
	; send 0x0F
	MOVB XAR6, #0x0F
	PUSH XAR6
	LC WRITE_COMMAND_REG
	; send 0x01
	MOVB XAR6, #0x01
	PUSH XAR6
	LC WRITE_COMMAND_REG
	LRET

I2C_WRITE_STRING:
	PUSH XAR1;
	PUSH XAR0;
	MOVL XAR0, #counter_string
	MOVL XAR1, #13
	MOVL *XAR0, XAR1

	MOVL XAR2, #in_string
LOOP_STRING:
	MOVL XAR6 , *XAR2++
	PUSH XAR6
	LC I2C_WRITE_1 ; RS
	LC I2C_WRITE_0 ; R/W = 0
	LC I2C_WRITE_BYTE
	DEC *XAR0
	B LOOP_STRING, NEQ
	LC I2C_STOP
	POP XAR0
	POP XAR1
	LRET






































