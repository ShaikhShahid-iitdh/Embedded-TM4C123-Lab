
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

// Function prototypes
void ADC_Init(void);
void LED_Init(void);
void delay_ms(uint32_t ms);
uint32_t ADC_Read(uint8_t channel);

#define LIGHT_THRESHOLD 2048  // Mid-range threshold (0-4095)
#define BALANCE_THRESHOLD 200  // Difference threshold for balanced detection

int main(void)
{
    uint32_t ldr1_value, ldr2_value;
    int32_t difference;

    // Initialize peripherals
    LED_Init();
    ADC_Init();

    // Startup test - blink all LEDs to confirm they work
    GPIO_PORTF_DATA_R = 0x0E;  // RED + BLUE + GREEN ON
    delay_ms(1000);
    GPIO_PORTF_DATA_R = 0x00;  // All OFF
    delay_ms(500);

    // Individual LED test
    GPIO_PORTF_DATA_R = 0x02;  // RED ON
    delay_ms(500);
    GPIO_PORTF_DATA_R = 0x04;  // BLUE ON
    delay_ms(500);
    GPIO_PORTF_DATA_R = 0x08;  // GREEN ON
    delay_ms(500);
    GPIO_PORTF_DATA_R = 0x00;  // All OFF
    delay_ms(500);

    while(1)
    {
        // Read LDR values
        ldr1_value = ADC_Read(0);  // PE3 = AIN0
        ldr2_value = ADC_Read(1);  // PE2 = AIN1

        // Clear all LEDs first
        GPIO_PORTF_DATA_R = 0x00;

        // RED LED: ON if LDR1 detects significant light
        if(ldr1_value > LIGHT_THRESHOLD) {
            GPIO_PORTF_DATA_R |= 0x02;  // Turn ON RED (PF1)
        }

        // BLUE LED: ON if LDR2 detects significant light
        if(ldr2_value > LIGHT_THRESHOLD) {
            GPIO_PORTF_DATA_R |= 0x04;  // Turn ON BLUE (PF2)
        }

        // Calculate difference
        difference = (int32_t)ldr1_value - (int32_t)ldr2_value;
        if(difference < 0) difference = -difference;

        // GREEN LED: ON when both LDRs are balanced
        if(difference < BALANCE_THRESHOLD) {
            GPIO_PORTF_DATA_R |= 0x08;  // Turn ON GREEN (PF3)
        }

        delay_ms(100);  // Update every 100ms
    }
}

void LED_Init(void)
{
    // Enable Port F clock
    SYSCTL_RCGCGPIO_R |= 0x20;
    while((SYSCTL_PRGPIO_R & 0x20) == 0);  // Wait until Port F is ready

    // Unlock PF0 (required for TM4C123GH6PM)
    GPIO_PORTF_LOCK_R = 0x4C4F434B;   // Unlock GPIO Port F
    GPIO_PORTF_CR_R = 0x1F;           // Allow changes to PF4-0

    // Configure PF1, PF2, PF3 as outputs (LEDs)
    GPIO_PORTF_DIR_R |= 0x0E;         // PF1, PF2, PF3 outputs
    GPIO_PORTF_DEN_R |= 0x0E;         // Enable digital function
    GPIO_PORTF_DATA_R = 0x00;         // Turn off all LEDs initially
}

void ADC_Init(void)
{
    // Enable ADC0 and Port E clock
    SYSCTL_RCGCADC_R |= 0x01;
    SYSCTL_RCGCGPIO_R |= 0x10;
    while((SYSCTL_PRADC_R & 0x01) == 0);    // Wait for ADC0 to be ready
    while((SYSCTL_PRGPIO_R & 0x10) == 0);   // Wait for Port E to be ready

    // Configure PE2 and PE3 as analog inputs
    GPIO_PORTE_AFSEL_R |= 0x0C;       // Enable alternate function
    GPIO_PORTE_DEN_R &= ~0x0C;        // Disable digital function
    GPIO_PORTE_AMSEL_R |= 0x0C;       // Enable analog function

    // Configure ADC0 Sample Sequencer 3
    ADC0_ACTSS_R &= ~0x08;            // Disable SS3 during configuration
    ADC0_EMUX_R &= ~0xF000;           // Software trigger
    ADC0_SSMUX3_R = 0;                // Default channel 0
    ADC0_SSCTL3_R = 0x06;             // End of sequence and interrupt enable
    ADC0_ACTSS_R |= 0x08;             // Enable SS3
}

uint32_t ADC_Read(uint8_t channel)
{
    ADC0_SSMUX3_R = channel & 0x0F;   // Select channel (0-15)
    ADC0_PSSI_R = 0x08;               // Initiate SS3
    while((ADC0_RIS_R & 0x08) == 0);  // Wait for conversion complete
    uint32_t result = ADC0_SSFIFO3_R & 0xFFF;  // Read 12-bit result
    ADC0_ISC_R = 0x08;                // Acknowledge completion
    return result;
}

void delay_ms(uint32_t ms)
{
    uint32_t i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 3180; j++);    // ~1ms delay at 16MHz
}
