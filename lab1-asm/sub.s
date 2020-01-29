@ lab1-asm/sub.s
@ Copyright (c) 2018-19 J. M. Spivey

        .syntax unified
        .global foo

        .text
        .thumb_func
foo:
@ ----------------
@ Two parameters are in registers r0 and r1

       subs r0, r0, r1         @ Add r0 and r1, result in r0
       @ eors r0, r0, r1	 @ Exclusive or each bit in r0 and r1, result in r0
       @ lsls r0, r0, r1	 @ Shift r0 left by r1, adds 0s, result in r0
				 @ Equivalent to multiplying r0 by 2^r1
       @ asrs r0, r0, r1	 @ Shift r0 right by r1, appends sign bit, result in r0
       @ rors r0, r0, r1	 @ Shift r0 right by r1, appends shifted out bit of r0,
				 @ result in r0
        
@ NOTE:
@ foo(2, 5) = -3
@ foo(0x2, 0x5) = 0xfffffffd

@ This is because -3 is represented as 0xfffffffd as an unsigned integer, because the greater half of all possible numbers (representable in 32 bits) is interpreted as negative.

@ Result is now in register r0
@ ----------------
        bx lr                   @ Return to the caller

