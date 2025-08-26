#include <stdint.h>
#include "tm4c123gh6pm.h"

void delayMs(int n);

int main(void)
{
    SYSCTL_RCGCGPIO_R = 0x20;
    while((SYSCTL_PRGPIO_R & 0x20) == 0);
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R = 0x02;
    GPIO_PORTF_DIR_R = 0x02;
    GPIO_PORTF_DEN_R = 0x02;
    while(1)
    {
        GPIO_PORTF_DATA_R = 0x02;
        delayMs(500);

        GPIO_PORTF_DATA_R = 0x00;
        delayMs(500);
    }
}

void delayMs(int n)
{
    int i, j;
    for(i = 0; i < n; i++)
        for(j = 0; j < 3180; j++);
}
