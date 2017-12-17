
.text

.extern kernel_rsp

.global syscall_entry

syscall_entry:
movq %rsp, %r10
movq kernel_rsp, %rsp

pushq %r10
pushq	%r11
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
popq %rcx
popq %r11
popq %rsp
sysretq