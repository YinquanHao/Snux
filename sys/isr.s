
.macro PUSHALL
    pushq 		%r8
    pushq 		%r9
    pushq 		%r10
    pushq 		%r11
    pushq 		%r12
    pushq 		%r13
    pushq 		%r14
    pushq 		%r15
    pushq 		%rax
    pushq 		%rbx
    pushq 		%rcx
    pushq 		%rdx
    pushq 		%rsi
    pushq 		%rdi
    pushq 		%rbp
    pushq 		%rsp
.endm

.macro POPALL
    popq		%rsp
    popq		%rbp
    popq		%rdi
    popq		%rsi
    popq		%rdx
    popq		%rcx
    popq		%rbx
    popq		%rax
    popq		%r15
    popq		%r14
    popq		%r13
    popq		%r12
    popq		%r11
    popq		%r10
    popq		%r9
    popq		%r8
.endm

.text


.extern timer_install

.extern handle_keyboard

.global irq0

irq0:
    	cli
	PUSHALL
	call timer_install
	POPALL
	sti
	iretq



.global irq1
irq1:
	cli
	PUSHALL
	call handle_keyboard
	POPALL
	sti
	iretq

.global isr0

isr0:
	iretq

.global isr1

isr1:
	iretq

.global isr2

isr2:
	iretq


