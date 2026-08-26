// Target Architecture: ARMv8-A (AArch64)

/* The start of this file is to help guide the linker
    .section - where code belongs in the compiled binary
    .global - marks _start so that it is visible outside the .asm file */

.section ".text.boot" 
.global _start

_start:
/*  Reading this register returns the current CPU Core ID no.
    If not Core 0, put the cores to sleep to prevent race conditions for resources*/
    mrs x0, mpidr_el1 
    and x0, x0, #0xFF
    cbz x0, active_core

inactive_core:
    wfe 
    b inactive_core

active_core:
/*  Kernel code will start executing from 0x80000 and move downards e.g. 0x80004, 0x80008 etc...
    Setting the sp to 0x8000 means it will grow downwards into lower unused RAM e.g. 0x7FFFF */
    ldr x0, =_start
    mov sp, x0

// Clears the BSS Section
    ldr x0, =__bss_start
    ldr w1, =__bss_size

1:
/*  Zero-ing memory:
    - cbz: if BSS 0, skip to label 2.
    - str: 8 bytes of 0s (xzr) and increment pointer
    - sub: Decrement loop counter w1 by 1
    - cbnz: Repeat until counter reaches 0. */
    cbz w1, 2f
    str xzr, [x0], #8
    sub w1, w1, #1
    cbnz w1, 1b

2:
    bl kernel_main // jump to C kernel

    b inactive_core // incase kernel_main ever returns, make the active core inactive


