#ifndef LEDBLINK_H
#define LEDBLINK_H

#include <stdint.h>
#include <mxc.h>

#define NUM_LEDS 8

typedef struct {
    mxc_gpio_regs_t *port;
    uint32_t pin;
} led_pin_t;

// Morse LED patterns (simple ON/OFF)
#define DOT_PATTERN      0xFF   // all on
#define DASH_PATTERN     0xFF   // all on (but held longer by repeated entries)
#define PAUSE_PATTERN    0x00   // all off

void LEDBank_Init(void);
void LED_Update(void);

#endif