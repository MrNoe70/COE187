#include <mxc.h>
#include <stdio.h>
#include "ledblink.h"

volatile int mode = 0;   // 0 = Morse, 1 = Shifting
volatile uint32_t last_button_time = 0;
#define DEBOUNCE_DELAY_MS 50

void gpio_isr(void *cbdata)
{
    uint32_t current_time = MXC_TMR_GetCount(MXC_TMR0);
    
    // Debounce check - ignore presses within DEBOUNCE_DELAY_MS
    if ((current_time - last_button_time) > (DEBOUNCE_DELAY_MS * 31)) {
        // 31 = (8MHz / 256 prescaler) / 1000 * DEBOUNCE_DELAY_MS
        // 8,000,000 / 256 = 31,250 Hz
        // 31,250 / 1000 = 31.25 ticks per ms
        // 31.25 * 50ms = ~1562 ticks for 50ms
        
        mode ^= 1;
        last_button_time = current_time;
        printf("Button pressed! Mode = %s\n", mode ? "SHIFT" : "MORSE");
    }
    
    MXC_GPIO_Handler(MXC_GPIO_GET_IDX(MXC_GPIO0));
}

void TMR0_IRQHandler(void)
{
    MXC_TMR_ClearFlags(MXC_TMR0);
    LED_Update();       // run one step every timer interrupt
}

int main(void)
{
    printf("\n--- LED Sequence Demo (MAX78000 FTHR) ---\n");

    // BUTTON SETUP (SW1)
    mxc_gpio_cfg_t sw;
    sw.port = MXC_GPIO0;
    sw.mask = MXC_GPIO_PIN_2;          // SW1
    sw.pad  = MXC_GPIO_PAD_PULL_UP;
    sw.func = MXC_GPIO_FUNC_IN;
    sw.vssel = MXC_GPIO_VSSEL_VDDIO;
    sw.drvstr = MXC_GPIO_DRVSTR_0;

    MXC_GPIO_Config(&sw);
    MXC_GPIO_RegisterCallback(&sw, gpio_isr, NULL);
    MXC_GPIO_IntConfig(&sw, MXC_GPIO_INT_FALLING);
    MXC_GPIO_EnableInt(sw.port, sw.mask);

    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(MXC_GPIO0)));

    // LED Setup
    LEDBank_Init();

    // TIMER CONFIG (100ms tick using Timer0)
    mxc_tmr_cfg_t tcfg;
    MXC_TMR_Shutdown(MXC_TMR0);
    tcfg.pres = TMR_PRES_256;
    tcfg.mode = TMR_MODE_CONTINUOUS;
    tcfg.bitMode = TMR_BIT_MODE_32;
    tcfg.clock = MXC_TMR_8M_CLK;
    tcfg.cmp_cnt = 3125; // 8MHz / 256 / 3125 = 10Hz (100ms)
    tcfg.pol = 0;

    MXC_TMR_Init(MXC_TMR0, &tcfg, 1);
    MXC_TMR_EnableInt(MXC_TMR0);
    NVIC_EnableIRQ(TMR0_IRQn);
    MXC_TMR_Start(MXC_TMR0);

    printf("System running. Press SW1 to change modes.\n");
    printf("Initial Mode: MORSE\n");
    printf("Debounce delay: %d ms\n", DEBOUNCE_DELAY_MS);

    while (1) {
        __WFI();      // sleep until interrupt
    }
}