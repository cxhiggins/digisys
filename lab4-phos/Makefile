# lab4-phos/Makefile
# Copyright (c) 2018 J. M. Spivey

all: phos.a examples

EXAMPLES = ex-echo.hex ex-heart.hex ex-level.hex ex-race.hex \
	ex-remote.hex ex-today.hex ex-analog.hex

PHOS = serial.o timer.o i2c.o radio.o adc.o phos.o mpx-m0.o lib.o startup.o

examples: $(EXAMPLES)

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar

CPU = -mcpu=cortex-m0 -mthumb
CFLAGS = -O -g -Wall -ffreestanding
# The -ffreestanding suppresses warnings that functions like exit()
# don't have types that match the built-in definitions.

# Use one interrupt handler for most interrupts
startup.o: CFLAGS += -DPHOS

phos.a: $(PHOS)
	$(AR) cr $@ $^

%.o: %.c
	$(CC) $(CPU) $(CFLAGS) -c $< -o $@ 

%.o: %.s
	$(AS) $(CPU) $< -o $@

%.hex: %.elf
	arm-none-eabi-objcopy -O ihex $< $@

%.elf: %.c phos.a
	$(CC) $(CPU) $(CFLAGS) -T NRF51822.ld -nostdlib \
	    $^ -lc -lgcc --specs=nano.specs -o $@ -Wl,-Map,$*.map

clean: force
	rm -f *.hex *.elf *.map *.o phos.a

.PHONY: examples

# Don't delete intermediate files
.SECONDARY:

force:

###

$(PHOS): phos.h
