// lab3-primes/primes.c
// Copyright (c) 2018 J. M. Spivey

#include "hardware.h"
#include "lib.h"
#include <stdarg.h>

#ifdef INTERRUPT
#define NBUF 64

static volatile int txidle;       // Whether UART is idle
static volatile int bufcnt = 0;   // Number of chars in buffer
static unsigned bufin = 0;        // Index of first free slot
static unsigned bufout = 0;       // Index of first occupied slot
static volatile char txbuf[NBUF]; // The buffer
#else
static int txinit;                // UART ready to transmit first char
#endif

/* serial_init -- set up UART connection to host */
void serial_init(void) {
    UART_ENABLE = 0;

    GPIO_DIRSET = BIT(USB_TX);
    GPIO_DIRCLR = BIT(USB_RX);
    SET_FIELD(GPIO_PINCNF[USB_TX], GPIO_PINCNF_PULL, GPIO_Pullup);
    SET_FIELD(GPIO_PINCNF[USB_RX], GPIO_PINCNF_PULL, GPIO_Pullup);

    UART_BAUDRATE = UART_BAUD_9600;     // 9600 baud
    UART_CONFIG = 0;                    // format 8N1
    UART_PSELTXD = USB_TX;              // choose pins
    UART_PSELRXD = USB_RX;
    UART_ENABLE = UART_Enabled;
    UART_STARTTX = 1;
    UART_STARTRX = 1;
    UART_RXDRDY = 0;
    UART_TXDRDY = 0;

#ifdef INTERRUPT
    // Interrupt for transmit only
    UART_INTENSET = BIT(UART_INT_TXDRDY);
    irq_priority(UART_IRQ, 3);
    enable_irq(UART_IRQ);
    txidle = 1;
#else
    txinit = 1;
#endif
}

#ifdef INTERRUPT

void uart_handler(void) {
    if (UART_TXDRDY) {
        UART_TXDRDY = 0;
        if (bufcnt == 0)
            txidle = 1;
        else {
            UART_TXD = txbuf[bufout];
            bufcnt--;
            bufout = (bufout+1) % NBUF;
        }
    }
}

/* serial_putc -- send output character */
void serial_putc(char ch) {
    while (bufcnt == NBUF) pause();

    intr_disable();
    if (txidle) {
        UART_TXD = ch;
        txidle = 0;
    } else {
        txbuf[bufin] = ch;
        bufcnt++;
        bufin = (bufin+1) % NBUF;
    }
    intr_enable();
}

#else

/* serial_putc -- send output character */
void serial_putc(char ch) {
    if (! txinit) {
        while (! UART_TXDRDY) { }
    }
    txinit = 0;
    UART_TXDRDY = 0;
    UART_TXD = ch;
}

#endif

void serial_printf(char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    do_print(serial_putc, fmt, va);
    va_end(va);
}

int modulo(int a, int b) {
    int r = a;
    while (r >= b) r -= b;
    return r;
}

int prime(int n) {
    for (int k = 2; k * k <= n; k++) {
        if (modulo(n, k) == 0)
            return 0;
    }

    return 1;
}

#define LEDS 0x0000fff0
#define LITE 0x00005fbf

void start_timer(void) {
    // Light an LED
    GPIO_DIRSET = LEDS;
    GPIO_OUT = LITE;

    // Start a timer
    TIMER0_MODE = TIMER_Mode_Timer;
    TIMER0_BITMODE = TIMER_32Bit;
    TIMER0_PRESCALER = 4; // Count at 1MHz
    TIMER0_START = 1;
}

void stop_timer(void) {
    // Turn LED off
    GPIO_OUT = 0;

    // Fetch timer result
    TIMER0_CAPTURE[0] = 1;
    unsigned time1 = TIMER0_CC[0];
#ifndef INTERRUPT
    serial_printf("%d millisec\r\n", (time1+500)/1000);
#else
    while (bufcnt > 0) pause();
    TIMER0_CAPTURE[0] = 1;
    unsigned time2 = TIMER0_CC[0];
    serial_printf("%d+%d millisec\r\n",
                  (time1+500)/1000, (time2-time1+500)/1000);
#endif

}

/* delay -- pause for n microseconds */
void delay(unsigned n) {
    unsigned t = n << 1;
    while (t > 0) {
        // 500nsec per iteration at 16MHz
        nop(); nop(); nop();
        t--;
    }
}

void init(void) {
    int n = 2, count = 0;

    serial_init();
    delay(10000);
    start_timer();

    while (count < 500) {
        if (prime(n)) {
            count++;
            serial_printf("prime(%d) = %d\r\n", count, n);
        }
        n++;
    }
    
    stop_timer();
}
