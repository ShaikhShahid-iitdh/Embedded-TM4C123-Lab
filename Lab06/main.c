#include <stdint.h>
#include "tm4c123gh6pm.h"

#define SYSCTL_BASE       0x400FE000
#define GPIO_PORTF_BASE   0x40025000
#define NVIC_BASE         0xE000E100
#define SYSTICK_BASE      0xE000E010
#define SYSCTL_RCGCGPIO   (*((volatile uint32_t *)(SYSCTL_BASE + 0x608)))
#define GPIO_PORTF_DATA   (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x3FC)))
#define GPIO_PORTF_DIR    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x400)))
#define GPIO_PORTF_DEN    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x51C)))
#define GPIO_PORTF_PUR    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x510)))
#define GPIO_PORTF_LOCK   (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x520)))
#define GPIO_PORTF_CR     (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x524)))
#define GPIO_PORTF_IS     (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x404)))
#define GPIO_PORTF_IBE    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x408)))
#define GPIO_PORTF_IEV    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x40C)))
#define GPIO_PORTF_ICR    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x41C)))
#define GPIO_PORTF_IM     (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x410)))
#define GPIO_PORTF_RIS    (*((volatile uint32_t *)(GPIO_PORTF_BASE + 0x414)))
#define NVIC_EN0          (*((volatile uint32_t *)(NVIC_BASE + 0x000)))
#define NVIC_ST_CTRL      (*((volatile uint32_t *)(SYSTICK_BASE + 0x00)))
#define NVIC_ST_RELOAD    (*((volatile uint32_t *)(SYSTICK_BASE + 0x04)))
#define NVIC_ST_CURRENT   (*((volatile uint32_t *)(SYSTICK_BASE + 0x08)))
#define RED_LED    (1U << 1)   
#define BLUE_LED   (1U << 2)   
#define GREEN_LED  (1U << 3)   
#define SW2        (1U << 0)   
#define SW1        (1U << 4)   

void GPIO_Handler(void) {
    SYSCTL_RCGCGPIO |= (1U << 5);
    while((SYSCTL_RCGCGPIO & (1U << 5)) == 0);
    GPIO_PORTF_LOCK = 0x4C4F434B;
    GPIO_PORTF_CR   = (RED_LED | BLUE_LED | GREEN_LED | SW1 | SW2);
    GPIO_PORTF_DIR  = (RED_LED | BLUE_LED | GREEN_LED);
    GPIO_PORTF_PUR  = (SW1 | SW2);
    GPIO_PORTF_DEN  = (RED_LED | BLUE_LED | GREEN_LED | SW1 | SW2);
    GPIO_PORTF_IS  &= ~(SW1 | SW2);   
    GPIO_PORTF_IBE &= ~(SW1 | SW2);   
    GPIO_PORTF_IEV &= ~(SW1 | SW2);   
    GPIO_PORTF_ICR  =  (SW1 | SW2);   
    GPIO_PORTF_IM  |=  (SW1 | SW2);  
    NVIC_EN0 |= (1U << 30);
}

void SysTick_Init(uint32_t ticks) {
    NVIC_ST_CTRL = 0;                
    NVIC_ST_RELOAD = ticks - 1;      
    NVIC_ST_CURRENT = 0;            
    NVIC_ST_CTRL = 0x07;             
}

void SysTick_Handler(void) {
    GPIO_PORTF_DATA ^= GREEN_LED;   D
}

void GPIOF_Handler(void) {
    if (GPIO_PORTF_RIS & SW1) {
        GPIO_PORTF_ICR = SW1;       
        GPIO_PORTF_DATA ^= BLUE_LED;  
    }
    if (GPIO_PORTF_RIS & SW2) {
        GPIO_PORTF_ICR = SW2;     
        GPIO_PORTF_DATA ^= RED_LED;   
    }
}

int main(void) {
    GPIO_Handler();
    SysTick_Init(8000000);
    while(1) {
    }
}
