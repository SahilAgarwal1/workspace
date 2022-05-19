	.def fletcher16 ;define function

	.ref Modulus255 ;reference to the c function which is called

	.thumb ; using the thumb instruction set
	.align 2 ; aligns to a 2 byte address boundry for 16 bit thumb instructions
	.text

fletcher16:

	.asmfunc ; function not label above

	push {r2-r7} ; push the registers which will be used SOE regs
	push {LR} ; push link reg bc we call another function inside

	;passed in variables
	MOV R4, R0 ; r4 is now the data pointer
	MOV R5, R1 ; r5 is now the count

	;0 out variables for function use
	MOV R6, #0 ; r6 is now 0d and sum1
	MOV R7, #0 ; r7 is 0 and = sum2

loop:
	CBZ r5, return ; branch to return if count is empty

	; accumulate data over sum1
	LDRB r0, [r4] ; using r0 as a temp register, load it with byte pointed too by r4
	ADD r0, r6, r0 ; sum1 = sum1 + temp
	BL Modulus255 ; call the modulus function
	MOV r6, r0 ; mov the modulod answer back into r6

	; accumulate data over sum2

	ADD r0, r7, r6; add sum1 to sum2
	BL Modulus255 ; modulus the answer
	MOV r7, r0

	;increment the data pointer and decrement the count
	ADD r4, r4, #1
	SUB r5,r5, #1

	B loop

return:

	LSL r7, r7, #8 ; shift the msb of sum2 by 8
	ORR r0, r7, r6 ; the return value

	; pop stuff back
	POP {LR}
	Pop {R2 - R7}

	BX LR

	.endasmfunc

	.align
	.end




