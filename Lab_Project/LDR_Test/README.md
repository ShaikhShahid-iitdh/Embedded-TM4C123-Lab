 * LDR Test Program - Simplified LED Indicator Method
 * Microcontroller: TM4C123GH6PM (Tiva C Series)
 * LDR connections: PE2 (AIN1) and PE3 (AIN0)
 * 
 * LED Indicators (using onboard RGB LED):
 * - RED LED (PF1): ON when LDR1 (PE3) > threshold
 * - BLUE LED (PF2): ON when LDR2 (PE2) > threshold  
 * - GREEN LED (PF3): ON when both LDRs are balanced
 * 
 * Note: PF0 requires unlocking on TM4C123GH6PM
