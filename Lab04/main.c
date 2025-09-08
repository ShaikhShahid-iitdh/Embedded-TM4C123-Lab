#include <stdint.h>

#define SYSCTL_RCGCGPIO   (*((volatile uint32_t *)0x400FE608))
#define GPIO_PORTF_DATA   (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_DIR    (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_DEN    (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_PUR    (*((volatile uint32_t *)0x40025510))
#define GPIO_PORTF_LOCK   (*((volatile uint32_t *)0x40025520))
#define GPIO_PORTF_CR     (*((volatile uint32_t *)0x40025524))

#define GPIO_LOCK_KEY     0x4C4F434B

#define LED_RED   (1 << 1)   // PF1
#define LED_BLUE  (1 << 2)   // PF2
#define SW2       (1 << 0)   // PF0
#define SW1       (1 << 4)   // PF4

int main(void)
{
    SYSCTL_RCGCGPIO |= (1 << 5);
    GPIO_PORTF_LOCK = GPIO_LOCK_KEY;
    GPIO_PORTF_CR |= LED_RED | LED_BLUE | SW1 | SW2;
    GPIO_PORTF_DIR |= (LED_RED | LED_BLUE);
    GPIO_PORTF_DIR &= ~(SW1 | SW2);
    GPIO_PORTF_PUR |= (SW1 | SW2);
    GPIO_PORTF_DEN |= (LED_RED | LED_BLUE | SW1 | SW2);

    while(1)
    {
        if((GPIO_PORTF_DATA & SW1) == 0)
            GPIO_PORTF_DATA |= LED_RED;
        else
            GPIO_PORTF_DATA &= ~LED_RED;

        if((GPIO_PORTF_DATA & SW2) == 0)
            GPIO_PORTF_DATA |= LED_BLUE;
        else
            GPIO_PORTF_DATA &= ~LED_BLUE;
    }
}
