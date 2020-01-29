#include "phos.h"
#include "hardware.h"

#define DISPLAY 15

static const unsigned short blank[] = { 0, 0, 0 };

static volatile const unsigned short *image = blank;

/* wait -- sleep until next timer ping */
static void wait(void) {
    message m;
    receive(TIMER, &m);
    assert(m.m_type == PING);
}

void display_task(int dummy) {
    int n = 0;

    GPIO_DIRSET = 0xfff0;
    priority(P_HIGH);
    timer_pulse(5);

    while (1) {
        GPIO_OUT = image[n];
        wait();
        if (++n == 3) n = 0;
    }
}

void display_set(const unsigned short *img) {
    image = img;
}

void display_init(void) {
    start(DISPLAY, "Display", display_task, 0, STACK);
}

#define ROW(r, c1, c2, c3, c4, c5, c6, c7, c8, c9) \
    (r | !c1 * BIT(4) | !c2 * BIT(5) | !c3 * BIT(6) | !c4 * BIT(7) \
     | !c5 * BIT(8) | !c6 * BIT(9) | !c7 * BIT(10) | !c8 * BIT(11) \
     | !c9 * BIT(12))

#define PIC(x11, x24, x12, x25, x13, \
            x34, x35, x36, x37, x38, \
            x22, x19, x23, x39, x21, \
            x18, x17, x16, x15, x14, \
            x33, x27, x31, x26, x32) \
    ROW(0x2000, x11, x12, x13, x14, x15, x16, x17, x18, x19),   \
    ROW(0x4000, x21, x22, x23, x24, x25, x26, x27, 0, 0),    \
    ROW(0x8000, x31, x32, x33, x34, x35, x36, x37, x38, x39)

static const unsigned short letter_a[] = {
    PIC(0,1,1,0,0,
        1,0,0,1,0,
        1,1,1,1,0,
        1,0,0,1,0,
        1,0,0,1,0)
};

static const unsigned short letter_b[] = {
    PIC(1,1,1,0,0,
        1,0,0,1,0,
        1,1,1,0,0,
        1,0,0,1,0,
        1,1,1,0,0)
};

void receiver_task(int dummy) {
    byte buf[32];
    int n;

    serial_printf("Hello\n");

    while (1) {
        n = radio_receive(buf);

        if (n == 1 && buf[0] == '1') {
            serial_printf("Button A\n");
            display_set(letter_a);
        } else if (n == 1 && buf[0] == '2') {
            serial_printf("Button B\n");
            display_set(letter_b);
        } else {
            serial_printf("Unknown packet, length %d\n", n);
        }
    }
}

void sender_task(int dummy) {
    GPIO_PINCNF[BUTTON_A] = 0;
    GPIO_PINCNF[BUTTON_B] = 0;

    while (1) {
        if ((GPIO_IN & BIT(BUTTON_A)) == 0)
            radio_send("1", 1);
        else if ((GPIO_IN & BIT(BUTTON_B)) == 0)
            radio_send("2", 1);

        timer_delay(100);
    }
}

void init(void) {
    radio_init();
    serial_init();
    timer_init();
    display_init();
    start(USER+0, "Receiver", receiver_task, 0, STACK);
    start(USER+1, "Sender", sender_task, 0, STACK);
}
