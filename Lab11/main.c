#include <stdint.h>
#include "uart_ee627.h"

int main(void)
{
    UART_setup();

    while (1)
    {
        UART_send(0x55);

        int i;
        for (i = 0; i < 1000000; i++) { }  // delay loop

        UART_send(0xAA);

        for (i = 0; i < 1000000; i++) { }  // delay loop
    }
}
