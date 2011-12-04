#include <msp430.h>
#include "TI_CC_include.h"
#include <stdint.h>
#include "radio_cc2500.h"

uint8_t txBuffer[4];

uint8_t rx_callback( uint8_t*, uint8_t );

int main(void)
{
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


  __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, enable interrupts

}

// This function is called to process the received packet
uint8_t rx_callback( uint8_t* buffer, uint8_t length )
{
  // Blink the LEDs after receiving a packet
	TI_CC_LED_PxOUT ^= 0x03;

  return 0;
}

// The ISR assumes the interrupt came from a press of one of the four buttons
// and therefore does not check the other four inputs.
#pragma vector=PORT1_VECTOR
__interrupt void port1_ISR (void)
{
  if(P1IFG & (TI_CC_SW1))
  {
    // Build packet
    txBuffer[0] = 2;                           // Packet length
    txBuffer[1] = 0x01;                        // Packet address
    txBuffer[2] = (~TI_CC_SW_PxIN) & 0x0F;     // Load four switch inputs

    cc2500_tx(txBuffer, 3);                 // Send value over RF
  }

  TI_CC_SW_PxIFG &= ~(TI_CC_SW1); // Clr flag that caused int

}
