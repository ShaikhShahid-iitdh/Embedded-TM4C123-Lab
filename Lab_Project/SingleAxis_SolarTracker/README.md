
 * ============================================================================
 * Single Axis Solar Tracker using TM4C123GH6PM
 * ============================================================================
 *
 * Hardware Components:
 * - TM4C123GH6PM Tiva C LaunchPad
 * - 2x LDR (Light Dependent Resistors)
 * - 2x 10kΩ resistors (for voltage divider)
 * - MG995 Servo Motor
 *
 * Pin Connections:
 * PE2 (AIN1) - LDR1 (Left sensor) via 10kΩ voltage divider
 * PE3 (AIN0) - LDR2 (Right sensor) via 10kΩ voltage divider
 * PB6 (M0PWM0) - Servo control signal
 *
 * Working Principle:
 * 1. Read light intensity from both LDR sensors using ADC
 * 2. Compare the light values
 * 3. Move servo toward the sensor with higher light intensity
 * 4. Stop when both sensors receive equal light
 * ============================================================================
