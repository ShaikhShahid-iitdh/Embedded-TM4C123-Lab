#include <stdint.h>
#include "tm4c123gh6pm.h"

void delayMs(int n);
int counter = 0;

int main(void)
{
    SYSCTL_RCGCGPIO_R = 0x20;
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R = 0x1F;
    GPIO_PORTF_DIR_R = 0x0E;
    GPIO_PORTF_PUR_R = 0x11;
    GPIO_PORTF_DEN_R = 0x1F;

    while(1)
    {
        if((GPIO_PORTF_DATA_R & 0x10) == 0)
        {
            if(counter == 0)
            {
                GPIO_PORTF_DATA_R = 0x02;
            }
            else if(counter == 1)
            {
                GPIO_PORTF_DATA_R = 0x08;
            }
            else if(counter == 2)
            {
                GPIO_PORTF_DATA_R = 0x04;
            }
            counter++;
            if(counter > 2) counter = 0;
            while((GPIO_PORTF_DATA_R & 0x10) == 0);
            delayMs(50);
        }
    }
}
void delayMs(int n)
{
    int i, j;
    for(i = 0; i < n; i++)
        for(j = 0; j < 3180; j++) {}
}
