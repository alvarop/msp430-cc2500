/*
 * main.c
 */
#include "device.h"
#include <stdint.h>

#define STROBE_PIN BIT2
#define RESET_PIN BIT1

void main(void) {

  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  // Setup oscillator for 16MHz operation
  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL = CALDCO_16MHZ;

  // Wait for changes to take effect
  __delay_cycles(4000);

  P1OUT = 0x00;
  P1DIR |= (STROBE_PIN+RESET_PIN);
  P1SEL |= (STROBE_PIN);

  // Pulse the reset pin for 1uS
  P1OUT |= RESET_PIN;
  __delay_cycles(16);
  P1OUT &= ~RESET_PIN;

  // Setup timer A
  // SMCLK, up mode
  TA0CTL = TASSEL_2 + MC_1 + ID_3 + TACLR;

  TACCTL1 = OUTMOD_7;
  TA0CCR0 = 9524; // 9524/2MHz = 4.762 ms strobe period
  TACCR1 = 524;   // 262uS strobe pulse


  for(;;) {
    __bis_SR_register( LPM1_bits + GIE );   // Enable interrupts and sleep

  }
}
