// level.c
// Copyright (c) 2019 J. M. Spivey

#include "phos.h"
#include "hardware.h"
#include <stddef.h>

/* Spirit level */

#define LED(i,j) ((0x1000 << i) | (~(0x8 << j) & 0x1ff0))

/* Table of LED settings for each 5x5 cell */
static const int ledval[][5] = {
    { LED(3,3), LED(2,7), LED(3,1), LED(2,6), LED(3,2) },
    { LED(1,8), LED(1,7), LED(1,6), LED(1,5), LED(1,4) },
    { LED(2,2), LED(1,9), LED(2,3), LED(3,9), LED(2,1) },
    { LED(3,4), LED(3,5), LED(3,6), LED(3,7), LED(3,8) },
    { LED(1,1), LED(2,4), LED(1,2), LED(2,5), LED(1,3) }
};

/* scale -- map acceleration to coordinate */
static int scale(int x) {
     if (x < -20) return 4;
     if (x < -10) return 3;
     if (x <= 10) return 2;
     if (x <= 20) return 1;
     return 0;
}

/* i2c_map -- use trial writes to map the I2C bus */
static void i2c_map(void) {
     message m;
     char buf = '\0';
     static const char hex[] = "0123456789abcdef";

     serial_printf("I2C map:\n");
     for (int i = 0; i < 8; i++) {
          for (int j = 0; j < 16; j++) {
               int a = 16*i + j;

               // Try writing a zero to address a
               m.m_type = WRITE;
               m.m_b1 = a;
               m.m_b2 = 1;
               m.m_b3 = 0;
               m.m_p2 = &buf;
               m.m_p3 = NULL;
               sendrec(I2C, &m);

               // Was anyone there?
               if (m.m_type == OK)
                    serial_printf(" %c%c", hex[i], hex[j]);
               else
                    serial_printf(" --");
          }
          serial_printf("\n");
     }
     serial_printf("\n");
}

/* trial -- map the I2C bus, then show the spirit level */
static void trial(int n) {
    int x, y, z;

     serial_printf("Hello\n\n");
     i2c_map();
     accel_start();
     
     GPIO_DIR = 0xfff0;

     while (1) {
          timer_delay(200);
          accel_reading(&x, &y, &z);
          serial_printf("x=%d y=%d z=%d\n", x, y, z);
          x = scale(x); y = scale(y);
          GPIO_OUT = ledval[y][x];
     }
}

void init(void) {
     serial_init();
     timer_init();
     i2c_init();
     start(USER+0, "Main", trial, 0, STACK);
}
