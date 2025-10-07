/* Task :
The Tiva Contains multiple Additional timer modules -- so called precision "Timers"
Your task is to read the datasheet to understand the configurations of these various Timers, and then
a) Use one of the Timers (NOT systick) to generate a PWM signal with fixed frequency (50kHz) and variable duty cycle.
b) The Duty cycle should be 50% at the beginning.
c) On each press of one of the push buttons the PWM duty should go up by 5%; and down by 5% for other push button.
d) Measure the output PWM signal on a scope to verify that duty cycle and period/frequency is as per the design.
*/

#include <stdint.h>
#include "tm4c123gh6pm.h"

#define F_CPU_HZ     16000000UL
#define PWM_FREQ_HZ  10000UL              // 10 kHz
#define PERIOD_TICKS (F_CPU_HZ / PWM_FREQ_HZ)  // 1600 ticks at 16 MHz

volatile uint16_t duty_high_ticks = 800;  // start ~50%
volatile uint16_t period_ticks    = PERIOD_TICKS; // 1600
volatile uint8_t  next_is_high    = 1;    // ISR will output HIGH first

static void delay(volatile uint32_t d){ while(d--){} }

// Timer0A ISR
void Timer0A_Handler(void)
{
    // Acknowledge timeout
    TIMER0_ICR_R = TIMER_ICR_TATOCINT;

    if (next_is_high) {
        // drive PF2 HIGH, schedule high-time
        GPIO_PORTF_DATA_R |=  (1U<<2);
        TIMER0_TAILR_R = duty_high_ticks - 1;  // high interval
        next_is_high = 0;
    } else {
        // drive PF2 LOW, schedule low-time
        GPIO_PORTF_DATA_R &= ~(1U<<2);
        uint16_t low_ticks = (period_ticks > duty_high_ticks) ? (period_ticks - duty_high_ticks) : 1;
        TIMER0_TAILR_R = low_ticks - 1;        // low interval
        next_is_high = 1;
    }

    // restart one-shot
    TIMER0_CTL_R |= TIMER_CTL_TAEN;
}

int main(void)
{
    // --- Clocks ---
    SYSCTL_RCGCGPIO_R |= (1U<<5);     // Port F clock
    SYSCTL_RCGCTIMER_R |= (1U<<0);    // Timer0 clock
    (void)SYSCTL_RCGCGPIO_R; (void)SYSCTL_RCGCTIMER_R; // allow a few cycles

    // --- PF2 as GPIO output (no alternate function) ---
    GPIO_PORTF_DIR_R  |=  (1U<<2);
    GPIO_PORTF_DEN_R  |=  (1U<<2);
    GPIO_PORTF_AFSEL_R &= ~(1U<<2);   // ensure GPIO, not CCP

    // --- Timer0A: one-shot, 32-bit, interrupt on timeout ---
    TIMER0_CTL_R  &= ~TIMER_CTL_TAEN;    // disable TimerA
    TIMER0_CFG_R   = 0x0;                // 32-bit mode
    TIMER0_TAMR_R  = (TIMER_TAMR_TAMR_1_SHOT); // one-shot mode
    TIMER0_IMR_R  |= TIMER_IMR_TATOIM;   // timeout interrupt
    }
}
    NVIC_EN0_R    |= (1U<<19);           // IRQ 19: Timer0A
            delay(10000);
        }

        }
        for (duty = PERIOD_TICKS; duty > 400; duty -= 50) {
            duty_high_ticks = (uint16_t)duty;
    // Prime: first edge will be HIGH interval
            delay(10000);
        for (duty = 400; duty < PERIOD_TICKS; duty += 50) {
            duty_high_ticks = (uint16_t)duty;   // ISR will pick up next edge
    next_is_high = 1;
    int duty;
    while (1) {
    TIMER0_TAILR_R = duty_high_ticks - 1;
    // --- Fade duty like your loop (400..1600) ---

