
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"      // Hardware memory map definitions
#include "inc/hw_types.h"        // Hardware type definitions
#include "inc/tm4c123gh6pm.h"    // Device specific definitions
#include "driverlib/sysctl.h"    // System control functions
#include "driverlib/gpio.h"      // GPIO functions
#include "driverlib/pin_map.h"   // Pin mapping definitions
#include "driverlib/pwm.h"       // PWM functions
#include "driverlib/adc.h"       // ADC functions

// ============================================================================
// CONFIGURATION PARAMETERS (Adjust these for your setup)
// ============================================================================
#define PWM_FREQUENCY       50      // Servo requires 50Hz PWM signal
#define SERVO_MIN_PULSE     1000    // 1ms pulse = 0° position
#define SERVO_MAX_PULSE     2000    // 2ms pulse = 180° position
#define SERVO_CENTER        1500    // 1.5ms pulse = 90° (center position)

#define LIGHT_THRESHOLD     100     // Minimum difference to trigger movement
#define SERVO_STEP          2       // Degrees to move per update
#define UPDATE_DELAY        100     // Delay between updates (milliseconds)

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================
uint32_t pwm_load;                  // PWM period value
uint32_t adc_left;                  // ADC value from left LDR
uint32_t adc_right;                 // ADC value from right LDR
int32_t servo_angle = 90;           // Current servo angle (0-180°)

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================
void InitializePWM(void);
void InitializeADC(void);
void SetServoAngle(int angle);
void ReadLightSensors(void);
void Delay_ms(uint32_t milliseconds);

// ============================================================================
// MAIN FUNCTION
// ============================================================================
int main(void)
{
    int32_t light_difference;

    // Step 1: Set system clock to 40 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL |
                   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // Step 2: Initialize peripherals
    InitializePWM();    // Setup PWM for servo control
    InitializeADC();    // Setup ADC for LDR readings

    // Step 3: Move servo to center position (90°)
    SetServoAngle(90);
    Delay_ms(1000);     // Wait for servo to reach position

    // Step 4: Main control loop
    while(1)
    {
        // Read light intensity from both sensors
        ReadLightSensors();

        // Calculate difference: positive means left is brighter
        light_difference = (int32_t)adc_left - (int32_t)adc_right;

        // Decision making based on light difference
        if(light_difference > LIGHT_THRESHOLD)
        {
            // Left sensor detects more light
            // Move servo counterclockwise (decrease angle)
            if(servo_angle > 0)
            {
                servo_angle -= SERVO_STEP;
                if(servo_angle < 0)
                    servo_angle = 0;

                SetServoAngle(servo_angle);
            }
        }
        else if(light_difference < -LIGHT_THRESHOLD)
        {
            // Right sensor detects more light
            // Move servo clockwise (increase angle)
            if(servo_angle < 180)
            {
                servo_angle += SERVO_STEP;
                if(servo_angle > 180)
                    servo_angle = 180;

                SetServoAngle(servo_angle);
            }
        }
        // If difference is within threshold, servo stays put

        // Wait before next reading
        Delay_ms(UPDATE_DELAY);
    }
}

// ============================================================================
// FUNCTION: InitializePWM
// Purpose: Configure PWM module for servo motor control
// PWM Pin: PB6 (Module 0, PWM Generator 0, Output 0)
// ============================================================================
void InitializePWM(void)
{
    uint32_t pwm_clock;

    // Enable PWM module 0 and GPIO Port B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Wait until peripherals are ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    // Configure PB6 as PWM output pin
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
    GPIOPinConfigure(GPIO_PB6_M0PWM0);

    // Configure PWM clock
    // System clock = 40 MHz, divide by 64 = 625 kHz PWM clock
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
    pwm_clock = SysCtlClockGet() / 64;

    // Calculate load value for 50Hz frequency
    // Load = (PWM Clock / Desired Frequency) - 1
    pwm_load = (pwm_clock / PWM_FREQUENCY) - 1;

    // Configure PWM generator 0 in count-down mode
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);

    // Set PWM period
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, pwm_load);

    // Set initial pulse width (center position)
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0,
                     (pwm_load * SERVO_CENTER) / 20000);

    // Enable PWM output
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);

    // Start PWM generator
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}

// ============================================================================
// FUNCTION: InitializeADC
// Purpose: Configure ADC for reading LDR sensors
// ADC Pins: PE2 (AIN1) for left LDR, PE3 (AIN0) for right LDR
// ============================================================================
void InitializeADC(void)
{
    // Enable ADC0 module and GPIO Port E
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Wait until peripherals are ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));

    // Configure PE2 and PE3 as ADC input pins
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_3);

    // Disable sample sequencer 3 before configuration
    ADCSequenceDisable(ADC0_BASE, 3);

    // Configure sample sequencer 3
    // Trigger: Software (processor trigger)
    // Priority: 0 (highest)
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
}

// ============================================================================
// FUNCTION: SetServoAngle
// Purpose: Set servo motor to specified angle (0-180 degrees)
// Parameter: angle - desired position in degrees
// ============================================================================
void SetServoAngle(int angle)
{
    uint32_t pulse_width;

    // Ensure angle is within valid range
    if(angle < 0)
        angle = 0;
    if(angle > 180)
        angle = 180;

    // Convert angle to pulse width (microseconds)
    // Formula: pulse_width = MIN_PULSE + (angle / 180) * (MAX_PULSE - MIN_PULSE)
    pulse_width = SERVO_MIN_PULSE +
                  ((angle * (SERVO_MAX_PULSE - SERVO_MIN_PULSE)) / 180);

    // Set PWM duty cycle to achieve desired pulse width
    // Total period = 20ms = 20000μs (for 50Hz)
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0,
                     (pwm_load * pulse_width) / 20000);
}

// ============================================================================
// FUNCTION: ReadLightSensors
// Purpose: Read analog values from both LDR sensors
// Updates: adc_left and adc_right global variables
// ============================================================================
void ReadLightSensors(void)
{
    uint32_t adc_buffer[1];

    // ---- Read Left LDR (PE2 - AIN1) ----

    // Configure sequencer step: Channel 1, interrupt enable, end of sequence
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0,
                            ADC_CTL_CH1 | ADC_CTL_IE | ADC_CTL_END);

    // Enable the sequence
    ADCSequenceEnable(ADC0_BASE, 3);

    // Clear interrupt flag
    ADCIntClear(ADC0_BASE, 3);

    // Trigger ADC conversion
    ADCProcessorTrigger(ADC0_BASE, 3);

    // Wait for conversion to complete
    while(!ADCIntStatus(ADC0_BASE, 3, false));

    // Read converted value
    ADCSequenceDataGet(ADC0_BASE, 3, adc_buffer);
    adc_left = adc_buffer[0];

    // Disable sequence
    ADCSequenceDisable(ADC0_BASE, 3);


    // ---- Read Right LDR (PE3 - AIN0) ----

    // Configure sequencer step: Channel 0, interrupt enable, end of sequence
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0,
                            ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);

    // Enable the sequence
    ADCSequenceEnable(ADC0_BASE, 3);

    // Clear interrupt flag
    ADCIntClear(ADC0_BASE, 3);

    // Trigger ADC conversion
    ADCProcessorTrigger(ADC0_BASE, 3);

    // Wait for conversion to complete
    while(!ADCIntStatus(ADC0_BASE, 3, false));

    // Read converted value
    ADCSequenceDataGet(ADC0_BASE, 3, adc_buffer);
    adc_right = adc_buffer[0];

    // Disable sequence
    ADCSequenceDisable(ADC0_BASE, 3);
}

// ============================================================================
// FUNCTION: Delay_ms
// Purpose: Simple delay function in milliseconds
// Parameter: milliseconds - delay duration
// Note: Uses SysCtlDelay which takes 3 clock cycles per iteration
// ============================================================================
void Delay_ms(uint32_t milliseconds)
{
    // Calculate delay cycles: (Clock / 3000) gives 1ms delay
    SysCtlDelay((SysCtlClockGet() / 3000) * milliseconds);
}

