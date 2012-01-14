#include <msp430.h>
#include "cc2500/TI_CC_include.h"
#include <stdint.h>
#include "cc2500/radio_cc2500.h"

uint8_t txBuffer[6];
static uint8_t rgb[3] = {128, 128, 128};

uint8_t rx_callback( uint8_t*, uint8_t );

int main(void)
{
  int8_t r, g ,b;
  r = 2;
  g = -3;
  b = 1;

  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  // Setup oscillator for 16MHz operation
  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL = CALDCO_16MHZ;

  // Wait for changes to take effect
  __delay_cycles(4000);

  setup_cc2500(rx_callback);

  // Configure ports -- switch inputs, LEDs, GDO0 to RX packet info from CCxxxx
  TI_CC_SW_PxIES = TI_CC_SW1;//Int on falling edge
  TI_CC_SW_PxIFG &= ~(TI_CC_SW1);//Clr flags
  TI_CC_SW_PxIE = TI_CC_SW1;//Activate enables

  TI_CC_LED_PxOUT &= ~(TI_CC_LED1 + TI_CC_LED2); //Outputs
  TI_CC_LED_PxDIR = TI_CC_LED1 + TI_CC_LED2; //Outputs


  __bis_SR_register(GIE);       // Enter LPM3, enable interrupts

  // Fade through colors
  for(;;)
  {

    if( (rgb[0] > 250) & (r > 0) )
    {
      r = -r;
    }
    if( (rgb[0] < 5) & (r < 0) )
    {
      r = -r;
    }

    if( (rgb[1] > 250) & (g > 0) )
    {
      g = -g;
    }
    if( (rgb[1] < 5) & (g < 0) )
    {
      g = -g;
    }

    if( (rgb[2] > 250) & (b > 0) )
    {
      b = -b;
    }
    if( (rgb[2] < 5) & (b < 0) )
    {
      b = -b;
    }

    rgb[0] += r;
    rgb[1] += g;
    rgb[2] += b;

    //Build packet
    txBuffer[0] = 4;                           // Packet length
    txBuffer[1] = 0x01;                        // Packet address
    txBuffer[2] = rgb[0];       // red
    txBuffer[3] = rgb[1];       // green
    txBuffer[4] = rgb[2];       // blue

    __delay_cycles(100000);
    __delay_cycles(100000);
    __delay_cycles(100000);
    __delay_cycles(100000);
    __delay_cycles(100000);

    // Send message to LED controller!
    cc2500_tx(txBuffer, 5);

    // Toggle local LED to signal transmission
    TI_CC_LED_PxOUT ^= 0x03;
  }

}

// This function is called to process the received packet
uint8_t rx_callback( uint8_t* buffer, uint8_t length )
{
  // Blink the LEDs after receiving a packet
	TI_CC_LED_PxOUT ^= 0x03;

  return 0;
}

// Port 1 ISR
#pragma vector=PORT1_VECTOR
__interrupt void port1_ISR (void)
{
  if(P1IFG & (TI_CC_SW1))
  {
    // Build packet
    txBuffer[0] = 4;                           // Packet length
    txBuffer[1] = 0x01;                        // Packet address
    txBuffer[2] = rgb[0];       // red
    txBuffer[3] = rgb[1];       // green
    txBuffer[4] = rgb[2];       // blue

    cc2500_tx(txBuffer, 5);                 // Send value over RF
  }

  TI_CC_SW_PxIFG &= ~(TI_CC_SW1); // Clr flag that caused int

}
