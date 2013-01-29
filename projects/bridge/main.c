/** @file rgb_controller.c
*
* @brief uart to cc2500 bridge
*
* @author Alvaro Prieto
*/
#include <stdint.h>
#include "device.h"
#include "uart.h"
#include "cc2500.h"

#define SERIAL_BUFFER_SIZE (32)

static uint8_t serial_buffer[SERIAL_BUFFER_SIZE];
static uint8_t packet_size = 0;

uint8_t cc2500_rx_callback( uint8_t*, uint8_t );
uint8_t uart_rx_callback( uint8_t );

void main(void)
{
  /* Init watchdog timer to off */
  WDTCTL = WDTPW|WDTHOLD;

  // Setup oscillator for 16MHz operation
  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL = CALDCO_16MHZ;

  // Setup LED outputs
  LED_PxOUT &= ~(LED1 | LED2);
  LED_PxDIR = LED1 | LED2; //Outputs

  // Wait for changes to take effect
  __delay_cycles(4000);

  // Make sure a function processes bytes received through UART
  setup_uart_callback( uart_rx_callback );

  // Setup CC2500 radio and register callback to process incoming packets
  setup_cc2500(cc2500_rx_callback);

  setup_uart();

  for(;;)
  {
   __bis_SR_register( LPM1_bits + GIE );   // Enable interrupts and sleep

   // Forward over cc2500
   if(packet_size > 1) {
	 // First byte in buffer is address, so we start sending from the second one
     cc2500_tx_packet(&serial_buffer[1], packet_size - 1, serial_buffer[0]);
     LED_PxOUT &= ~(LED1);
   }
   packet_size = 0;
  }

}

// This function is called to process the received packet
uint8_t cc2500_rx_callback( uint8_t* buffer, uint8_t length )
{
  LED_PxOUT |= LED2;
  // Escape and forward over serial
  uart_write_escaped(buffer, length);
  LED_PxOUT &= ~(LED2);
  return 0;
}

//
// void serial_callback( uint8_t command )
// Receive serial packets, decode them, and then process them
//
uint8_t uart_rx_callback( uint8_t rx_byte )
{
  static uint8_t receiving_packet;
  static uint8_t escape_next_character;
  static uint8_t buffer_index;

  if( receiving_packet )
  {
    if( escape_next_character ) {
      escape_next_character = 0;
      serial_buffer[buffer_index++] = rx_byte ^ 0x20;
    }
    else if ( ESCAPE_BYTE == rx_byte ) {
      escape_next_character = 1;
    }
    else if ( START_BYTE == rx_byte ) {

    }
    else if ( END_BYTE == rx_byte ) {
      packet_size = buffer_index;
      receiving_packet = 0;
      buffer_index = 0;

      // wake up!!
      return 1;

    }
    else {
      serial_buffer[buffer_index++] = rx_byte;
    }
  } else if ( START_BYTE == rx_byte) {
    receiving_packet = 1;
    LED_PxOUT |= LED1;
  } else {
    buffer_index = 0;
  }

  // Make sure the buffer doesn't overflow
  if( buffer_index == SERIAL_BUFFER_SIZE )
  {
    LED_PxOUT &= ~(LED1);
    buffer_index = 0;
    receiving_packet = 0;
  }

  return 0;
}
