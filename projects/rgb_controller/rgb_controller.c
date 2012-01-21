/** @file rgb_controller.c
*
* @brief Get RGB data via 19200 baud serial connection and send out via radio
*
* @author Alvaro Prieto
*/
#include <stdint.h>
#include "device.h"
#include "uart.h"
#include "cc2500.h"

uint8_t txBuffer[6];
uint8_t buffer_index;
static uint8_t rgb[3] = {0, 0, 0};

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

   // Make sure a function processes bytes received through UART
   setup_uart_callback( uart_rx_callback );

   // Setup CC2500 radio and register callback to process incoming packets
   setup_cc2500(cc2500_rx_callback);

   setup_uart();

   // Setup LED outputs
   LED_PxOUT &= ~(LED1); //Outputs
   LED_PxDIR = LED1; //Outputs


   // Build packet
   txBuffer[0] = 4;         // Packet length
   txBuffer[1] = 0x01;      // Packet address
   buffer_index = 0;

   for(;;)
   {
     __bis_SR_register( LPM1_bits + GIE );   // Enable interrupts and sleep

   }

}

// This function is called to process the received packet
uint8_t cc2500_rx_callback( uint8_t* buffer, uint8_t length )
{
  // Currently not used
  return 0;
}

//
// Get 3 bytes of data from uart and send RGB values over radio
// This does not check incoming data. If a byte is missed, all of the
// colors will be shifted. I will fix this later...
//
uint8_t uart_rx_callback( uint8_t rx_byte )
{
  // Fill up buffer as we receive each color
  rgb[buffer_index++] = rx_byte;

  // Once all three colors have been received, build and send message
  if(buffer_index == 3)
  {
    buffer_index = 0;

    // Modify packet in buffer RGB data
    txBuffer[2] = rgb[0];       // red
    txBuffer[3] = rgb[1];       // green
    txBuffer[4] = rgb[2];       // blue
    LED_PxOUT ^= 0x01;

    // Send out packet with RGB values
    cc2500_tx(txBuffer, 5);
  }

  return 0;
}
