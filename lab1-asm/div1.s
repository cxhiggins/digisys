@ lab1-asm/sub.s
@ Copyright (c) 2018-19 J. M. Spivey

        .syntax unified
        .global foo

        .text
        .thumb_func
foo:
@ ----------------
@ Two parameters are in registers r0 and r1

        movs r2, #1		@ Initializes r2 to #0
	cmp r1, #0		@ Checks if the divisor is 0
	beq done		@ Skips to return if divisor is 0

loop:
@ ----------------
@ Two parameters are in registers r0 and r1
@ r1 is a relatively small constant
	movs r3, r2
	muls r3, r1, r3
	cmp r0, r3		@ Compares r0 and r3, sets N = 1 if r0 < r3
	bmi done
	adds r2, r2, #1
	b loop

@ Result is now in register r0
done:
@ ----------------
	subs r0, r2, #1
        bx lr                   @ Return to the caller

