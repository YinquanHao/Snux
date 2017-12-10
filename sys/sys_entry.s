
.text

.extern kernel_rsp

.global syscall_entry

syscall_entry:
#cli
# swapgs
# movq
#xchg %bx, %bx 
movq %rsp, %r10
movq kernel_rsp, %rsp
pushq %r10
# pushq	$0x23	/* regs->ss */
# pushq	PER_CPU_VAR(rsp_scratch)	/* regs->sp */
pushq	%r11	#regs->flags 
# pushq	$0x2B	/* regs->cs */
pushq    %rcx



pushq    %rax
pushq    %rdi
pushq    %rsi
pushq    %rdx
pushq    %rcx
pushq    %r8
pushq    %r9
pushq    %r10

pushq  %r12
pushq  %r13
pushq  %r14
pushq  %r15
pushq  %rbp
pushq  %rbx
movq %rsp, %rdi	
callq	syscall_handler
popq %rbx
popq %rbp
popq %r15
popq %r14
popq %r13
popq %r12
popq %r10
popq %r9
popq %r8
popq %rcx
popq %rdx
popq %rsi
popq %rdi
addq $0x8,%rsp 
#popq %rax
popq %rcx
popq %r11
#xchg %bx, %bx 
#movq %r10, %rsp
popq %rsp
#addq $48 ,%rsp 
sysretq