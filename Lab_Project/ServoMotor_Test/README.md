/*
 * MG995 Servo Motor Test Program for TM4C123GH6PM
 * 
 * Connections:
 * - Servo Signal (Orange/Yellow) -> PB6 (M0PWM0)
 * - Servo VCC (Red) -> External 5V supply
 * - Servo GND (Brown) -> GND (common with Tiva)
 * 
 * This program sweeps the servo from 0° to 180° and back
 * PWM Frequency: 50Hz (20ms period)
 * Pulse Width: 1ms (0°) to 2ms (180°)
 */
