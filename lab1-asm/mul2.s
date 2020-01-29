@ lab1-asm/sub.s
@ Copyright (c) 2018-19 J. M. Spivey

        .syntax unified
        .global foo

        .text
        .thumb_func
foo:
@ ----------------
@ Two parameters are in registers r0 and r1

       movs r2, #0		 @ Initializes r2 to #0

mul:
@ ----------------
@ Two parameters are in registers r0 and r1
@ r1 is a relatively small constant

       cmp  r1, #1		 @ Compares r1 and #1, sets Z = 1 if r1 == #1
       beq done

       lsrs r1, r1, #1		 @ r1 / 2 -> r1
       blo rest			 @ Branches if !C (i.e. if r1 was even)
       adds r2, r2, r0		 @ r0 + r2 -> r2 (accounts for r1 being odd)

rest:
       lsls r0, r0, #1		 @ r0 * 2 -> r0
       b mul			 @ Restarts loop

@ Result is now in register r0
done:
@ ----------------
	adds r0, r0, r2		@ r0 + r2 -> r0
        bx lr                   @ Return to the caller

