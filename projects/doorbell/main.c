/*
* main.c -- Doorbell Example
*/

#include <stdint.h>
#include "device.h"
#include "spi.h"
#include "cc2500.h"

uint8_t cc2500_rx_callback( uint8_t*, uint8_t );

void main(void) {

	WDTCTL = WDTPW + WDTHOLD; // Stop WDT

	// Setup oscillator for 16MHz operation
	BCSCTL1 = CALBC1_16MHZ;
	DCOCTL = CALDCO_16MHZ;

	// Wait for changes to take effect
	__delay_cycles(4000);

	// Setup CC2500 radio and register callback to process incoming packets
	setup_cc2500(cc2500_rx_callback);

  // Turn radio off to save power
	void cc2500_sleep();

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

      __delay_cycles(100000);
          __delay_cycles(100000);
          __delay_cycles(100000);
          __delay_cycles(100000);
          __delay_cycles(100000);

      // Send 'doorbell on' packet
      cc2500_tx_packet((uint8_t *)"doorbell on", sizeof("doorbell on"), 0x00);

    } else {
      // Turn off the led
      P1OUT &= ~BIT0;

      // Send 'doorbell off' packet
      cc2500_tx_packet((uint8_t *)"doorbell off", sizeof("doorbell off"), 0x00);

      // Turn off the radio to save power
      void cc2500_sleep( );

      // Enter LPM1, sleeeep
      __bis_SR_register(LPM3_bits);
    }

  }

}

//
// Right now we're only sending data, so we don't have to do anything with
// incoming messages
//
uint8_t cc2500_rx_callback( uint8_t* buffer, uint8_t length )
{

  return 0;
}


#pragma vector=PORT1_VECTOR
__interrupt void port1_isr(void)
{
  if( P1IFG & BIT2 )
  {
    // Wakeup!
    __bic_SR_register_on_exit(LPM3_bits);

    P1IFG &= ~BIT2; // Clear interrupt flag
  }
}
