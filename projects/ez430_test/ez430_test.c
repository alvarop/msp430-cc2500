/** @file ez430_test.c
*
* @brief Get RGB data via 19200 baud serial connection and send out via radio
*
* @author Alvaro Prieto
*/

#include <msp430.h>
#include "cc2500/TI_CC_include.h"
#include <stdint.h>
#include "cc2500/radio_cc2500.h"

uint8_t txBuffer[6];
uint8_t buffer_index;
static uint8_t rgb[3] = {128, 128, 128};

uint8_t rx_callback( uint8_t*, uint8_t );

/*******************************************************************************
 * @fn     void setup_uart( void )
 * @brief  configure uart for 19200BAUD on ports 1.6 and 1.7
 * ****************************************************************************/
void setup_uart( void )
{
  //
  // NOTE on port selection. p3.0 is the a slave enable output for SPI
  // but instead of using the built in functions to use it, it will be
  // controlled as a regular I/O pin
  //
  P3DIR |= BIT4;                      // Set P3.4 as TX output
  P3SEL |= BIT4 + BIT5;               // Select UART/SPI function

  //
  // Configure Serial Port
  //
  UCA0CTL1 |= UCSWRST;                // **Put state machine in reset**
  UCA0CTL1 |= UCSSEL_2;               // CLK = SMCLK
  UCA0BR0 = 52;                      // 1MHz/19200=52.08 (see User's Guide)
  UCA0BR1 = 0x00;                     //
  UCA0MCTL = UCBRS_0+UCBRF_0;         // Modulation UCBRSx=0, UCBRFx=0
  UCA0CTL1 &= ~UCSWRST;               // **Initialize USCI state machine**

  IE2 |= UCA0RXIE;                    // Enable USCI_A0/B0 RX interrupt

}

void uart_put_char( uint8_t character )
{
  while (!(IFG2&UCA0TXIFG));  // USCI_A0 TX buffer ready?
  UCA0TXBUF = character;
}

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  // Setup oscillator for 12MHz operation
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;

  // Wait for changes to take effect
  __delay_cycles(4000);

  setup_cc2500(rx_callback);

  setup_uart();

  P2SEL = 0;                                // Sets P2.6 & P2.7 as GPIO

  TI_CC_LED_PxOUT &= ~(TI_CC_LED1 + TI_CC_LED2); //Outputs
  TI_CC_LED_PxDIR = TI_CC_LED1 + TI_CC_LED2; //Outputs


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
uint8_t rx_callback( uint8_t* buffer, uint8_t length )
{
  // Blink the LEDs after receiving a packet
  //TI_CC_LED_PxOUT ^= 0x03;

  return 0;
}

//
// Get 3 bytes of data from uart and send RGB values over radio
//
static uint8_t uart_rx_callback( uint8_t rx_byte )
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

#pragma vector=USCIAB0RX_VECTOR
__interrupt void uart_rx_isr (void)
{

  // Process incoming byte from USART
  if( IFG2 & UCA0RXIFG )
  {
    TI_CC_LED_PxOUT ^= 0x02;

    // Call rx callback function
    if( uart_rx_callback( UCA0RXBUF ) )
    {
      // If function returns something nonzero, wakeup the processor
      __bic_SR_register_on_exit(LPM1_bits);
    }


  }
  // Process incoming byte from SPI
  else if ( IFG2 & UCB0RXIFG )
  {

  }
}
