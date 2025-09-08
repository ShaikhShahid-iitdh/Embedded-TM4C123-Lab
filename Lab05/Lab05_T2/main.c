#include <stdint.h>

// System Control and GPIO registers
#define SYSCTL_RCGC2_R      (*((volatile uint32_t *)0x400FE108))
#define GPIO_PORTF_DATA_R   (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_DIR_R    (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_DEN_R    (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_LOCK_R   (*((volatile uint32_t *)0x40025520))
#define GPIO_PORTF_CR_R     (*((volatile uint32_t *)0x40025524))
#define GPIO_PORTF_PUR_R    (*((volatile uint32_t *)0x40025510))

#define GPIO_LOCK_KEY       0x4C4F434B  // Unlock key for GPIO

// SysTick registers
#define SYSTICK_CTRL        (*((volatile uint32_t *)0xE000E010))
#define SYSTICK_RELOAD      (*((volatile uint32_t *)0xE000E014))
#define SYSTICK_CURRENT     (*((volatile uint32_t *)0xE000E018))

// GPIO pin definitions
#define RED_LED   (1U << 1)
#define BLUE_LED  (1U << 2)
#define SW1       (1U << 4)

int main(void) {
    // Enable clock for Port F
    SYSCTL_RCGC2_R |= 0x20;

    // Unlock and allow changes on Port F pins
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R |= (RED_LED | BLUE_LED | SW1);

    // Set directions: LEDs as output, switch as input
    GPIO_PORTF_DIR_R |= (RED_LED | BLUE_LED);
    GPIO_PORTF_DIR_R &= ~SW1;

    // Enable digital functionality
    GPIO_PORTF_DEN_R |= (RED_LED | BLUE_LED | SW1);

    // Enable pull-up resistor for SW1
    GPIO_PORTF_PUR_R |= SW1;

    // Configure SysTick for 1-second delay (assuming 16 MHz clock)
    SYSTICK_RELOAD = 16000000 - 1;
    SYSTICK_CURRENT = 0;
    SYSTICK_CTRL = (1U << 2) | (1U << 0);

    while (1) {
        // Toggle RED LED every second
        if (SYSTICK_CTRL & (1U << 16)) {
            GPIO_PORTF_DATA_R ^= RED_LED;
        }

        // Turn on BLUE LED if switch is pressed
        if ((GPIO_PORTF_DATA_R & SW1) == 0) {
            GPIO_PORTF_DATA_R |= BLUE_LED;
        } else {
            GPIO_PORTF_DATA_R &= ~BLUE_LED;
        }
    }
}
