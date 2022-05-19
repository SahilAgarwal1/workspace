; G8RTOS_SchedulerASM.s
; Holds all ASM functions needed for the scheduler
; Note: If you have an h file, do not have a C file and an S file of the same name

	; Functions Defined
	.def G8RTOS_Start, PendSV_Handler

	; Dependencies
	.ref CurrentlyRunningThread, G8RTOS_Scheduler, StartCriticalSection, EndCriticalSection

	.thumb		; Set to thumb mode
	.align 2	; Align by 2 bytes (thumb mode uses allignment by 2 or 4)
	.text		; Text section

; Need to have the address defined in file 
; (label needs to be close enough to asm code to be reached with PC relative addressing)
RunningPtr: .field CurrentlyRunningThread, 32

; G8RTOS_Start
;	Sets the first thread to be the currently running thread
;	Starts the currently running thread by setting Link Register to tcb's Program Counter
G8RTOS_Start:

	.asmfunc

	; load stack pointer with the stack pointer from the currently running thread
	ldr r4, RunningPtr
	ldr r4, [r4] ; dereference the ptr
	ldr sp, [r4, #8]


	; Pops registers back in the order of the exception return table
	pop {r4-r11, r0-r3, r12}

	; skip
	pop {lr}
	pop {lr}

	;enable interrupts
	cpsie i

	; branch back to thread
	bx lr

	.endasmfunc

; PendSV_Handler
; - Performs a context switch in G8RTOS
; 	- Saves remaining registers into thread stack
;	- Saves current stack pointer to tcb
;	- Calls G8RTOS_Scheduler to get new tcb
;	- Set stack pointer to new stack pointer from new tcb
;	- Pops registers from thread stack
PendSV_Handler:
	
	.asmfunc
	;starting a critical session
	PUSH {r12, lr} ; push r12 and the link register to preserve them from the function
	bl StartCriticalSection;
	POP{r12, lr}


	;saving registers that arent auto saved to the thread stack
	PUSH {r4-r11}

	; save current stack pointer to tcb
	ldr r4, RunningPtr ; runningPTR is a pointer to the current thread control block pointer
	ldr r4, [r4] ; we dereference runningPTR once to get the pointer to the thread control block
	str sp, [r4, #8] ; now str the stack pointer to the sp element in the tcb

	;calling G8TROS_Scheduler to get the new tcb

	push {r0, r12, lr}
	bl G8RTOS_Scheduler
	pop{r0,r12,lr}

	;set stack pointer to new pointer from new tcb
	ldr r4, RunningPtr ; running pointer is a pounter to the tcb pointer
	ldr r4, [r4] ; dereference the pointer
	ldr sp, [r4, #8] ; now the sp has taken the sp element from the tcb


	; pop the registers from the stack
	pop {r4-r11}

	; end critical section
	push {r12 , lr}
	bl EndCriticalSection
	pop {r12, lr}

	; branch back

	bx lr

	.endasmfunc
	
	; end of the asm file
	.align
	.end
