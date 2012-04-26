/*
* main.c -- Doorbell Example
*/

#include <stdint.h>
#include "device.h"
#include "cc2500.h"

void main(void) {

	WDTCTL = WDTPW + WDTHOLD; // Stop WDT

	// Setup oscillator for 16MHz operation
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL = CALDCO_16MHZ;

	// Wait for changes to take effect
	__delay_cycles(4000);

  // Set P1.0 as an output
	P1OUT &= ~(BIT0);
	P1DIR |= BIT0;

  // Set P1.2 as an input
	P1DIR &= ~BIT2;

	// Enable pull-up resistor on P1.2
	P1OUT |= BIT2;
	P1REN |= BIT2;

	// Enable interrupts on P1.2
  P1IES |= BIT2;  // Set to interrupt on falling edge
	P1IFG &= ~BIT2; // Clear interrupt flag
	P1IE |= BIT2;   // Enable interrupt

	// enable interrupts
  __bis_SR_register(GIE);

  for(;;)
  {

    // Debounce for 1ms
    __delay_cycles(16000);

    // Check if the doorbell is ringing
    if(!(P1IN & BIT2))
    {
      // Turn on the LED
      P1OUT |= BIT0;

      // Send radio message here

    } else {
      // Turn off the led
      P1OUT &= ~BIT0;

      // Enter LPM1, sleeeep
      __bis_SR_register(LPM1_bits);
    }

  }

}

#pragma vector=PORT1_VECTOR
__interrupt void port1_isr(void)
{
  if( P1IFG & BIT2 )
  {
    // Wakeup!
    __bic_SR_register_on_exit(LPM1_bits);

    P1IFG &= ~BIT2; // Clear interrupt flag
  }
}
