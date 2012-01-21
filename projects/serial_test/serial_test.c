/** @file serial_test.c
*
* @brief Get RGB data via 19200 baud serial connection and send out via radio
*
* @author Alvaro Prieto
*/
#include <stdint.h>
#include "cc2500/TI_CC_include.h"
#include "cc2500/radio_cc2500.h"
#include "uart.h"

uint8_t txBuffer[6];
uint8_t buffer_index;
static uint8_t rgb[3] = {128, 128, 128};

uint8_t cc2500_rx_callback( uint8_t*, uint8_t );
uint8_t uart_rx_callback( uint8_t );


void main(void)
{
  /* Init watchdog timer to off */
   WDTCTL = WDTPW|WDTHOLD;

   // Setup oscillator for 16MHz operation
   BCSCTL1 = CALBC1_16MHZ;
   DCOCTL = CALDCO_16MHZ;

   // Wait for changes to take effect
   __delay_cycles(4000);


   // Set LED output to pin 1.0
   P1DIR |= BIT0;

   setup_uart_callback( uart_rx_callback );

   setup_cc2500(cc2500_rx_callback);

   setup_uart();

   P2SEL = 0;                                // Sets P2.6 & P2.7 as GPIO

   TI_CC_LED_PxOUT &= ~(TI_CC_LED1); //Outputs
   TI_CC_LED_PxDIR = TI_CC_LED1; //Outputs


   //Build packet
   txBuffer[0] = 4;                           // Packet length
   txBuffer[1] = 0x01;                        // Packet address
   buffer_index = 0;

   for(;;)
   {
     __bis_SR_register( LPM1_bits + GIE );   // Enable interrupts and sleep

   }

}

// This function is called to process the received packet
uint8_t cc2500_rx_callback( uint8_t* buffer, uint8_t length )
{
  // Blink the LEDs after receiving a packet
  //TI_CC_LED_PxOUT ^= 0x03;

  return 0;
}

//
// Get 3 bytes of data from uart and send RGB values over radio
//
uint8_t uart_rx_callback( uint8_t rx_byte )
{

  rgb[buffer_index++] = rx_byte;

  if(buffer_index == 3)
  {
    buffer_index = 0;
    txBuffer[2] = rgb[0];       // red
    txBuffer[3] = rgb[1];       // green
    txBuffer[4] = rgb[2];       // blue
    TI_CC_LED_PxOUT ^= 0x01;
    cc2500_tx(txBuffer, 5);

    //uart_put_char('.');
  }

  return 0;
}
