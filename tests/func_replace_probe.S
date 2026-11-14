.text
.globl probe
.type probe,@function
probe:
    push %rbp
    push %rbx
    push %r12
    push %r13
    push %r14
    push %r15
    push %rdi
    mov %rsp, expected_rsp(%rip)
    mov $1, %rax
    mov $2, %rbx
    mov $3, %rcx
    mov $4, %rdx
    mov $5, %rsi
    mov $6, %rdi
    mov $7, %rbp
    mov $8, %r8
    mov $9, %r9
    mov $10, %r10
    mov $11, %r11
    mov $12, %r12
    mov $13, %r13
    mov $14, %r14
    mov $15, %r15
    pushq $0x246
    popfq
    call *(%rsp)
    mov %rax, observed(%rip)
    mov %rbx, observed+8(%rip)
    mov %rcx, observed+16(%rip)
    mov %rdx, observed+24(%rip)
    mov %rsi, observed+32(%rip)
    mov %rdi, observed+40(%rip)
    mov %rbp, observed+48(%rip)
    mov %r8, observed+56(%rip)
    mov %r9, observed+64(%rip)
    mov %r10, observed+72(%rip)
    mov %r11, observed+80(%rip)
    mov %r12, observed+88(%rip)
    mov %r13, observed+96(%rip)
    mov %r14, observed+104(%rip)
    mov %r15, observed+112(%rip)
    mov %rsp, observed+120(%rip)
    pushfq
    popq observed+128(%rip)
    add $8, %rsp
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbx
    pop %rbp
    ret
.size probe, .-probe
.section .note.GNU-stack,"",@progbits
