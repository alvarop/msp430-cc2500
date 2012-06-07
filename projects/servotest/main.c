/*
 * main.c
 */
#include "device.h"
#include <stdint.h>

void main(void) {

  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  // Setup oscillator for 8MHz operation
  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL = CALDCO_16MHZ;

  // Wait for changes to take effect
  __delay_cycles(4000);

  P1DIR |= BIT2;
  P1SEL |= BIT2;

  // Setup timer A
  // SMCLK, up mode
  TA0CTL = TASSEL_2 + MC_3 + ID_3 + TACLR;

  TA0CCR0 = 50000; // 100000/2MHz = 25 ms * 2 =~ 50ms (up-down)

  TACCR1 = 47550;
  TACCTL1 = OUTMOD_7;
  uint32_t delay;
  for(;;) {
    TACCR1 = 47550;

    for(delay=0; delay < ((10/*-ish seconds*/) * 1000); delay++)
    {
      __delay_cycles(16000);

    }

    TACCR1 = 46500;

    for(delay=0; delay < ((10/*-ish seconds*/) * 1000); delay++)
    {
      __delay_cycles(16000);

    }

  }
}
