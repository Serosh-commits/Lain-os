.section .text.entry
.global _start
_start:
    csrw sie, zero
    la sp, stack_top
    call kmain
    j .
.section .bss
.align 16
stack_bottom: .skip 16384
stack_top:
