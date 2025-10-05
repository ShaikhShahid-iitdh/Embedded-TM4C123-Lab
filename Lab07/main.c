#include <stdint.h>
#include "tm4c123gh6pm.h"
void SysTick_Handler(void);
void GPIOF_Handler(void);
void SysTick_Init(void);
void PortF_Init(void);

int main(void)
{
    PortF_Init();
    SysTick_Init();
    __asm("    CPSIE I\n");

    while(1)
    {
    }
}
void GPIOF_Handler(void)
{
    if (GPIO_PORTF_RIS_R & 0x01) {

void SysTick_Init(void)
{
    NVIC_ST_CTRL_R = 0;
    NVIC_ST_RELOAD_R = 16000000 - 1;
    NVIC_ST_CURRENT_R = 0;
    NVIC_ST_CTRL_R = 0x07;
}
void PortF_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x20;
    while((SYSCTL_PRGPIO_R & 0x20) == 0){};
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R = 0x1F;
    GPIO_PORTF_DIR_R = 0x0E;
    GPIO_PORTF_DEN_R = 0x1F;
    GPIO_PORTF_PUR_R = 0x11;
    GPIO_PORTF_IS_R &= ~0x11;
    GPIO_PORTF_IBE_R &= ~0x11;
    GPIO_PORTF_IEV_R &= ~0x11;
    GPIO_PORTF_ICR_R = 0x11;
    GPIO_PORTF_IM_R |= 0x11;
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF3FFFFF) | 0x00A00000;
    NVIC_EN0_R |= 0x40000000;
}

void SysTick_Handler(void)
{
    GPIO_PORTF_DATA_R ^= 0x02;
}

void GPIOF_Handler(void)
{
    if (GPIO_PORTF_RIS_R & 0x01) {
        GPIO_PORTF_ICR_R = 0x01;
        GPIO_PORTF_DATA_R ^= 0x04;
    }
}
