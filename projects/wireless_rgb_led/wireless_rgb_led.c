#include <msp430.h>
#include "TI_CC_include.h"
#include <stdint.h>
#include "radio_cc2500.h"

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

  setup_cc2500(rx_callback);

  TI_CC_LED_PxOUT &= ~(TI_CC_LED1 + TI_CC_LED2); //Outputs
  TI_CC_LED_PxDIR = TI_CC_LED1 + TI_CC_LED2; //Outputs

  // Configure ports -- switch inputs, LEDs, GDO0 to RX packet info from CCxxxx
   P2OUT |= (BIT0 | BIT1 | BIT2);
   P2DIR |= BIT0 | BIT1 | BIT2;

   // Setup timer A
   // SMCLK, up mode, enable interrupt
   TA0CTL = TASSEL_2 + MC_1 + TAIE + TACLR;
   TA0CCR0 = 512; // 512/16MHz = ~32us

  __bis_SR_register(LPM1_bits + GIE);       // Enter LPM3, enable interrupts

}

// This function is called to process the received packet
uint8_t rx_callback( uint8_t* buffer, uint8_t length )
{
  // copy colors from buffer
  rgb[0] = buffer[1]; // Copy red
  rgb[1] = buffer[2]; // Copy green
  rgb[2] = buffer[3]; // Copy blue

  // Don't wake up the processor
  return 0;
}

// Timer A isr
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TA1_ISR (void)
{
  static uint8_t counter;

  // Increase virtual timer counter
  counter++;

  // Process red
  if ( counter >= rgb[0] )
  {
    P2OUT |= 0x01;
  }
  else
  {
    P2OUT &= ~0x01;
  }

  // Process green
  if ( counter >= rgb[1] )
  {
    P2OUT |= 0x02;
  }
  else
  {
    P2OUT &= ~0x02;
  }

  // process blue
  if ( counter >= rgb[2] )
  {
    P2OUT |= 0x04;
  }
  else
  {
    P2OUT &= ~0x04;
  }

  // Clear interrupt flag
  TA0IV &= ~TA0IV_TAIFG;

}
