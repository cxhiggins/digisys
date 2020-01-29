@ lab1-asm/mul1.s
@ Copyright (c) 2018-19 J. M. Spivey

        .syntax unified
        .global foo

        .text
        .thumb_func
foo:
@ ----------------
@ Multiplication by repeated addition
@ Variables x = r0, y = r1, z = r2
@ Invariant a * b = x * y + z

        muls r0, r1, r0

@ ----------------
        bx lr
