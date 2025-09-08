#include <stdint.h>

// System Control and GPIO Port F base addresses
#define SYSCTL_RCGCGPIO   (*((volatile uint32_t *)0x400FE608))
#define GPIO_PORTF_DATA   (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_DIR    (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_DEN    (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_PUR    (*((volatile uint32_t *)0x40025510))
#define GPIO_PORTF_LOCK   (*((volatile uint32_t *)0x40025520))
#define GPIO_PORTF_CR     (*((volatile uint32_t *)0x40025524))

// Unlock value
#define GPIO_LOCK_KEY     0x4C4F434B

// GPIO pin definitions (Port F)
#define LED_RED   (1U << 1)   // PF1
#define LED_BLUE  (1U << 2)   // PF2
#define SW2       (1U << 0)   // PF0
#define SW1       (1U << 4)   // PF4

int main(void)
{
    // Enable Port F clock
    SYSCTL_RCGCGPIO |= (1U << 5);

    // Unlock and allow changes to PF0
    GPIO_PORTF_LOCK = GPIO_LOCK_KEY;
    GPIO_PORTF_CR |= (LED_RED | LED_BLUE | SW1 | SW2);

    // Set direction: LEDs as output, switches as input
    GPIO_PORTF_DIR |= (LED_RED | LED_BLUE);
    GPIO_PORTF_DIR &= ~(SW1 | SW2);

    // Enable pull-up resistors for switches
    GPIO_PORTF_PUR |= (SW1 | SW2);

    // Enable digital function for LEDs and switches
    GPIO_PORTF_DEN |= (LED_RED | LED_BLUE | SW1 | SW2);

    while (1)
    {
        // SW1 pressed -> RED ON, else RED OFF
        if ((GPIO_PORTF_DATA & SW1) == 0U)
            GPIO_PORTF_DATA |= LED_RED;
        else
            GPIO_PORTF_DATA &= ~LED_RED;

        // SW2 pressed -> BLUE ON, else BLUE OFF
        if ((GPIO_PORTF_DATA & SW2) == 0U)
            GPIO_PORTF_DATA |= LED_BLUE;
        else
            GPIO_PORTF_DATA &= ~LED_BLUE;
    }
}
