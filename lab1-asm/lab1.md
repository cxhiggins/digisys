# Lab 1

### DigiSys

##### subs

2 - 5 seems to return a large unsigned number because the unsigned version of -3 is actually 0xfffffffd.

##### mul1 vs mul2

mul1:

> Gimme a number: -12312
>
> Gimme a number: -213123
>
> foo(-12312, -213123) = -1670996920
>
> foo(0xffffcfe8, 0xfffcbf7d) = 0x9c669c48
>
> 293 cycles, 18 microsec
>
> 
>
> Gimme a number: -100
>
> Gimme a number: -100
>
> foo(-100, -100) = 10000
>
> foo(0xffffff9c, 0xffffff9c) = 0x2710
>
> 292 cycles, 18 microsec



mul2:

> Gimme a number: -12312
>
> Gimme a number: -213123
>
> foo(-12312, -213123) = -1670996920
>
> foo(0xffffcfe8, 0xfffcbf7d) = 0x9c669c48
>
> 4 cycles, 0 microsec
>
> 
>
> Gimme a number: -100
>
> Gimme a number: -100
>
> foo(-100, -100) = 10000
>
> foo(0xffffff9c, 0xffffff9c) = 0x2710
>
> 4 cycles, 0 microsec

The built-in multiply instruction used in mul3 seems to operate in constant time, which is much faster than the code executed in mul2.