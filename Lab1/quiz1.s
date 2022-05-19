	.def quiz1 ;define function

	.thumb ; using the thumb instruction set
	.align 2 ; aligns to a 2 byte address boundry for 16 bit thumb instructions
	.text

quiz1:

	.asmfunc ; function not label above

	PUSH {r2-r11} ; push the registers that will not be used
	PUSH {LR} ; push the link register

	; load the data pointed too by the struct pointer

	LDRB r4, [r0 , #0] ; load r4 with the offset 0, loads in the hours
	LDRB r5, [r0 ,#1] ; load r5 with pointer offset 1, loads in the minutes
	LDRB r6, [r0 ,#2] ; loads r6 with the day
	LDRB r7, [r0 ,#3] ; loads r7 with the month

	; calculate remaining minutes
	MOV r8, #60 ; minutes in an hour
	SUB r5, r8, r5 ; subtract minutes left from hour

	; calculate remaining hours
	MOV r8, #24 ; hours in a day
	SUB r4, r8, r4 ; hours left in the day
	SUB r4, r4, #1 ; extra hour subtraction to account for the minutes that completed the next hour

	; calculate the remaining days
	MOV r8, #30 ; days in a month
	sub r6, r8, r6
	sub r6, r6, #1 ; subtract 1 for the day that the hours completed

	; calculate the remaining months
	MOV r8, #12 ; months in a year
	sub r7, r8, r7
	sub r7, r7, #1 ; subtract 1 for the month that the days complete

	; Store the calculated values back into the struct pointer
	STRB r4, [r1,  #0] ; load r4 with the offset 0, loads in the hours
	STRB r5, [r1, #1] ; load r5 with pointer offset 1, loads in the minutes
	STRB r6, [r1 , #2] ; loads r6 with the day
	STRB r7, [r1 , #3] ; loads r7 with the month

	; pop stuff back
	POP {LR}
	POP {r2-r11}

	BX LR

	.endasmfunc

	.align
	.end









