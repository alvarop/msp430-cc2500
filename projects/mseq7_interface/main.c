/*
 * main.c
 */
#include "device.h"
#include <stdint.h>

void main(void) {

  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  // Setup oscillator for 16MHz operation
  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL = CALDCO_16MHZ;

  // Wait for changes to take effect
  __delay_cycles(4000);

  P1DIR |= BIT2;
  P1SEL |= BIT2;

  // Setup timer A
  // SMCLK, up mode
  TA0CTL = TASSEL_2 + MC_1 + ID_3 + TACLR;

  TA0CCR0 = 9524; // 9524/2MHz = 4.762 ms

  TACCR1 = 524;
  TACCTL1 = OUTMOD_7;

  for(;;) {


  }
}
