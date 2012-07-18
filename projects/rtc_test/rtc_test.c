/** @file wireless_rgb_led.c
*
* @brief Radio test
*
* @author Alvaro Prieto
*/
#include <stdint.h>
#include "device.h"
#include "cc2500.h"

uint8_t rx_callback( uint8_t*, uint8_t );

static uint8_t rgb[3] = {0, 0, 0};

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD; // Stop WDT

  // Setup oscillator for 16MHz operation
  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL = CALDCO_16MHZ;

  // Wait for changes to take effect
  __delay_cycles(4000);

  // Initialize cc2500 and register callback function to process incoming data
  setup_cc2500(rx_callback);

  LED_PxOUT &= ~(LED1 + LED2); //Outputs
  LED_PxDIR = LED1 + LED2; //Outputs

  // Configure ports -- switch inputs, LEDs, GDO0 to RX packet info from CCxxxx
   P2OUT |= (BIT0 | BIT1 | BIT2);
   P2DIR |= BIT0 | BIT1 | BIT2;

  __bis_SR_register(LPM1_bits + GIE);       // Enter LPM3, enable interrupts

}

// This function is called to process the received packet
uint8_t rx_callback( uint8_t* buffer, uint8_t length )
{

  LED_PxOUT ^= 0x01; // Toggle LED on message received

  // Don't wake up the processor
  return 0;
}
