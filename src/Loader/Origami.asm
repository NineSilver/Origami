bits 64

default rel
section .text

global JumpToKernel
JumpToKernel:
    mov rdi, rcx
    mov rsp, rdx

    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx
    xor rsi, rsi

    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15

    jmp r8
