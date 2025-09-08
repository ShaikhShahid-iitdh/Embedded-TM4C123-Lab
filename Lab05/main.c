#include <stdint.h>

// System Control and GPIO Registers
#define SYSCTL_RCGC2_R      (*((volatile uint32_t *)0x400FE108))
#define GPIO_PORTF_DATA_R   (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_DIR_R    (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_DEN_R    (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_LOCK_R   (*((volatile uint32_t *)0x40025520))
#define GPIO_PORTF_CR_R     (*((volatile uint32_t *)0x40025524))
#define GPIO_LOCK_KEY       0x4C4F434B

// SysTick Registers
#define STCTRL              (*((volatile uint32_t *)0xE000E010))
#define STRELOAD            (*((volatile uint32_t *)0xE000E014))
#define STCURRENT           (*((volatile uint32_t *)0xE000E018))

// SysTick Control Bits
#define STCTRL_ENABLE       (1U << 0)
#define STCTRL_INTEN        (1U << 1)
#define STCTRL_CLK_SRC      (1U << 2)
#define STCTRL_COUNT        (1U << 16)

// LED Pin (PF1)
#define RED_LED             (1U << 1)

int main(void) {
    // Enable clock for Port F
    SYSCTL_RCGC2_R |= 0x20;

    // Unlock Port F and allow changes
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R |= RED_LED;

    // Set PF1 as output and enable digital function
    GPIO_PORTF_DIR_R |= RED_LED;
    GPIO_PORTF_DEN_R |= RED_LED;

    // Configure SysTick for 0.5s delay (assuming 16 MHz clock)
    STRELOAD = 8000000 - 1; // 8 million cycles
    STCURRENT = 0;
    STCTRL = STCTRL_CLK_SRC | STCTRL_ENABLE;

    while (1) {
        // Wait until COUNT flag is set
        while ((STCTRL & STCTRL_COUNT) == 0);

        // Toggle LED
        GPIO_PORTF_DATA_R ^= RED_LED;
    }
}
