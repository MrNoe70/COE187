#include <mxc.h>
#include <stdio.h>
#include "ledblink.h"

volatile int mode = 0;   // 0 = Morse, 1 = Shifting
volatile uint32_t last_button_time = 0;
#define DEBOUNCE_DELAY_TICKS 1563  // 50ms in timer ticks

void gpio_isr(void *cbdata)
{
    // FIRST: Clear the interrupt immediately
    MXC_GPIO_Handler(MXC_GPIO_GET_IDX(MXC_GPIO0));
    
    // SECOND: Check if button is actually pressed (active low)
    if (MXC_GPIO_InGet(MXC_GPIO0, MXC_GPIO_PIN_2) == 0) {
        uint32_t current_time = MXC_TMR_GetCount(MXC_TMR0);
        uint32_t time_diff;
        
        // Handle timer overflow (32-bit timer)
        if (current_time >= last_button_time) {
            time_diff = current_time - last_button_time;
        } else {
            // Timer overflow occurred
            time_diff = (0xFFFFFFFF - last_button_time) + current_time;
        }
        
        // Debounce check - ignore presses within 50ms
        if (time_diff > DEBOUNCE_DELAY_TICKS) {
            mode = 1 - mode;  // Toggle between 0 and 1
            last_button_time = current_time;
            printf("Button pressed! Mode = %s\n", mode ? "SHIFT" : "MORSE");
            
            // Reset pattern steps when mode changes
            if (mode == 0) {
                morse_step = 0;  // Reset Morse to beginning
            } else {
                shift_step = 0;  // Reset Shift to beginning
            }
        }
    }
    // If button is not pressed (pin is high), it was a false trigger - ignore it
}

void TMR0_IRQHandler(void)
{
    MXC_TMR_ClearFlags(MXC_TMR0);
    LED_Update();       // run one step every timer interrupt
}

int main(void)
{
    // Enable all clocks first
    MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
    SystemCoreClockUpdate();
    
    printf("\n--- LED Sequence Demo (MAX78000 FTHR) ---\n");

    // BUTTON SETUP (SW1) - FIXED CONFIGURATION
    mxc_gpio_cfg_t sw;
    sw.port = MXC_GPIO0;
    sw.mask = MXC_GPIO_PIN_2;          // SW1
    sw.pad  = MXC_GPIO_PAD_PULL_UP;    // Strong pull-up
    sw.func = MXC_GPIO_FUNC_IN;
    sw.vssel = MXC_GPIO_VSSEL_VDDIO;
    sw.drvstr = MXC_GPIO_DRVSTR_0;

    // Configure GPIO first
    MXC_GPIO_Config(&sw);
    
    // IMPORTANT: Set pin to input with pull-up BEFORE enabling interrupt
    MXC_GPIO_OutSet(MXC_GPIO0, MXC_GPIO_PIN_2);  // Ensure output high if configured as output
    
    // Clear any pending interrupts first
    MXC_GPIO_ClearFlags(MXC_GPIO0, MXC_GPIO_PIN_2);
    
    // Configure interrupt for BOTH edges to catch noise
    MXC_GPIO_RegisterCallback(&sw, gpio_isr, NULL);
    MXC_GPIO_IntConfig(&sw, MXC_GPIO_INT_BOTH);  // Changed from FALLING to BOTH
    
    // Enable interrupt AFTER configuration
    MXC_GPIO_EnableInt(MXC_GPIO0, MXC_GPIO_PIN_2);

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
    printf("Debounce delay: 50 ms\n");
    printf("Interrupt configured for BOTH edges\n");

    while (1) {
        __WFI();      // sleep until interrupt
    }
}