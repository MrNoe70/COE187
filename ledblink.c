#include "ledblink.h"
#include <stdio.h>
#include <mxc.h>

extern volatile int mode;

const led_pin_t leds[NUM_LEDS] = {
    {MXC_GPIO0, MXC_GPIO_PIN_17},
    {MXC_GPIO0, MXC_GPIO_PIN_16},
    {MXC_GPIO3, MXC_GPIO_PIN_1},
    {MXC_GPIO0, MXC_GPIO_PIN_19},
    {MXC_GPIO0, MXC_GPIO_PIN_11},
    {MXC_GPIO0, MXC_GPIO_PIN_8},
    {MXC_GPIO1, MXC_GPIO_PIN_1},
    {MXC_GPIO2, MXC_GPIO_PIN_3},
};

// Morse C-O-E pattern (step-by-step)
static const uint32_t morse_pattern[] = {
    // C: -.-.
    DOT_PATTERN, PAUSE_PATTERN,  // Dot
    DOT_PATTERN, PAUSE_PATTERN,  // Dot  
    DOT_PATTERN, PAUSE_PATTERN,  // Dot (simplified for timing)
    DOT_PATTERN, PAUSE_PATTERN,  // Dot

    // O: ---
    DOT_PATTERN, DOT_PATTERN, DOT_PATTERN,  // Dash (repeated dots)
    PAUSE_PATTERN,
    DOT_PATTERN, DOT_PATTERN, DOT_PATTERN,  // Dash
    PAUSE_PATTERN,
    DOT_PATTERN, DOT_PATTERN, DOT_PATTERN,  // Dash
    PAUSE_PATTERN,

    // E: .
    DOT_PATTERN, PAUSE_PATTERN,

    // Word gap
    PAUSE_PATTERN, PAUSE_PATTERN, PAUSE_PATTERN
};

static const int morse_len = sizeof(morse_pattern) / sizeof(morse_pattern[0]);

// Shifting LED pattern
static const uint32_t shift_pattern[] = {
    0b11111110,
    0b11111101,
    0b11111011,
    0b11110111,
    0b11101111,
    0b11011111,
    0b10111111,
    0b01111111,
};

static const int shift_len = 8;

static volatile int morse_step = 0;
static volatile int shift_step = 0;

static void set_leds(uint32_t pattern)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        if (pattern & (1 << i))
            MXC_GPIO_OutSet(leds[i].port, leds[i].pin);
        else
            MXC_GPIO_OutClr(leds[i].port, leds[i].pin);
    }
}

void LEDBank_Init(void)
{
    mxc_gpio_cfg_t cfg;

    for (int i = 0; i < NUM_LEDS; i++) {
        cfg.port = leds[i].port;
        cfg.mask = leds[i].pin;
        cfg.pad = MXC_GPIO_PAD_NONE;
        cfg.func = MXC_GPIO_FUNC_OUT;
        cfg.vssel = MXC_GPIO_VSSEL_VDDIO;
        cfg.drvstr = MXC_GPIO_DRVSTR_0;
        MXC_GPIO_Config(&cfg);
        MXC_GPIO_OutClr(leds[i].port, leds[i].pin);
    }

    printf("LED Bank Initialized\n");
}

void LED_Update(void)
{
    if (mode == 0) {
        set_leds(morse_pattern[morse_step]);
        morse_step = (morse_step + 1) % morse_len;
    } else {
        set_leds(shift_pattern[shift_step]);
        shift_step = (shift_step + 1) % shift_len;
    }
}