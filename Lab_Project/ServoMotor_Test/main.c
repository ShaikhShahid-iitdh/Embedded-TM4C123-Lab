#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

// =======================================================
// === CONFIGURATION ===
// =======================================================
#define SERVO_PWM_BASE      PWM0_BASE
#define SERVO_PWM_GEN       PWM_GEN_0
#define SERVO_PWM_OUT       PWM_OUT_0
#define SERVO_PWM_OUT_BIT   PWM_OUT_0_BIT

#define SERVO_MIN_US        1000   // 1.0 ms pulse width
#define SERVO_MAX_US        2000   // 2.0 ms pulse width
#define SERVO_STEP_US       10     // microsecond step per correction
#define PWM_FREQ_HZ         50     // 50Hz for hobby servo

#define LDR_DEADBAND        40     // ADC difference threshold (tune experimentally)
#define MOVE_DELAY_MS       400    // minimum delay between movements

// =======================================================
// === GLOBAL VARIABLES ===
// =======================================================
volatile uint32_t msTicks = 0;
uint32_t g_ui32PwmPeriod = 0;
uint32_t currentPulseUs = 1500;

// =======================================================
// === SYSTICK HANDLER (for timing) ===
// =======================================================
void SysTick_Handler(void) {
    msTicks++;
}

void delay_ms(uint32_t ms) {
    uint32_t start = msTicks;
    while ((msTicks - start) < ms);
}

// =======================================================
// === CONVERT MICROSECONDS TO PWM TICKS ===
// =======================================================
uint32_t usToTicks(uint32_t us) {
    uint32_t period_us = 1000000 / PWM_FREQ_HZ;
    uint64_t ticks = ((uint64_t)g_ui32PwmPeriod * us) / period_us;
    if (ticks > g_ui32PwmPeriod) ticks = g_ui32PwmPeriod;
    return (uint32_t)ticks;
}

// =======================================================
// === SERVO CONTROL ===
// =======================================================
void setServoPulseUs(uint32_t us) {
    if (us < SERVO_MIN_US) us = SERVO_MIN_US;
    if (us > SERVO_MAX_US) us = SERVO_MAX_US;
    currentPulseUs = us;
    PWMPulseWidthSet(SERVO_PWM_BASE, SERVO_PWM_OUT, usToTicks(us));
}

void stepServo(int direction) {
    if (direction > 0 && currentPulseUs < SERVO_MAX_US)
        setServoPulseUs(currentPulseUs + SERVO_STEP_US);
    else if (direction < 0 && currentPulseUs > SERVO_MIN_US)
        setServoPulseUs(currentPulseUs - SERVO_STEP_US);
}

// =======================================================
// === ADC READ (for LDRs) ===
// =======================================================
void readLDR(uint32_t *ldrLeft, uint32_t *ldrRight) {
    uint32_t adcValues[2];

    ADCProcessorTrigger(ADC0_BASE, 0);
    while(!ADCIntStatus(ADC0_BASE, 0, false));
    ADCSequenceDataGet(ADC0_BASE, 0, adcValues);
    ADCIntClear(ADC0_BASE, 0);

    *ldrLeft  = adcValues[0];  // AIN0 (PE3)
    *ldrRight = adcValues[1];  // AIN1 (PE2)
}

// =======================================================
// === MAIN INITIALIZATION ===
// =======================================================
int main(void) {

    // --- System Clock 50 MHz ---
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL |
                   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // --- SysTick for ms counter ---
    SysTickPeriodSet(SysCtlClockGet() / 1000);
    SysTickEnable();
    SysTickIntEnable();

    // --- Enable peripherals ---
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));

    // --- Configure PWM ---
    GPIOPinConfigure(GPIO_PB6_M0PWM0);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    uint32_t pwmClock = SysCtlClockGet() / 64;
    g_ui32PwmPeriod = pwmClock / PWM_FREQ_HZ;

    PWMGenConfigure(SERVO_PWM_BASE, SERVO_PWM_GEN, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(SERVO_PWM_BASE, SERVO_PWM_GEN, g_ui32PwmPeriod);
    setServoPulseUs(1500); // center
    PWMOutputState(SERVO_PWM_BASE, SERVO_PWM_OUT_BIT, true);
    PWMGenEnable(SERVO_PWM_BASE, SERVO_PWM_GEN);

    // --- Configure ADC (PE3=Ain0, PE2=Ain1) ---
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    ADCSequenceDisable(ADC0_BASE, 0);
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH1 | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 0);
    ADCIntClear(ADC0_BASE, 0);

    // =======================================================
    // === MAIN LOOP ===
    // =======================================================
    while(1) {
        uint32_t ldrLeft = 0, ldrRight = 0;
        readLDR(&ldrLeft, &ldrRight);

        int32_t diff = (int32_t)ldrLeft - (int32_t)ldrRight;

        if (diff > LDR_DEADBAND) {
            // Left brighter -> turn left
            stepServo(+1);
        } else if (diff < -LDR_DEADBAND) {
            // Right brighter -> turn right
            stepServo(-1);
        }
        delay_ms(MOVE_DELAY_MS);
    }
}

