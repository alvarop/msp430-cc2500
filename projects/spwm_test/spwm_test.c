#include <msp430.h>
#include <stdint.h>

// Determines LED brightness from 0 to 255
static volatile uint8_t brightness = 0;

int main(void)
{
  uint8_t mode = 1;

  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  // Setup oscillator for 16MHz operation
  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL = CALDCO_16MHZ;

  // Wait for changes to take effect
  __delay_cycles(4000);

  // Configure ports -- switch inputs, LEDs, GDO0 to RX packet info from CCxxxx
  P2OUT |= (BIT0 | BIT1 | BIT2);
  P2DIR |= BIT0 | BIT1 | BIT2;

  // Setup timer A
  // SMCLK, up mode
  TA0CTL = TASSEL_2 + MC_1 + TAIE + TACLR;
  TA0CCR0 = 512; // 512/16MHz = ~32us

  __bis_SR_register(GIE);       // Enter LPM3, enable interrupts

  // Fade the LED's on and off
  for(;;)
  {
    __delay_cycles(50000);
    if( mode )
    {
      if( ++brightness == 255 )
      {
        mode = 0;
      }
    }
    else
    {
      if( --brightness == 0 )
      {
        mode = 1;
      }
    }
  }

}

// Timer A isr
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TA1_ISR (void)
{
  static uint8_t counter;

  // Increase virtual timer counter
  counter++;

  if ( counter > brightness )
  {
    P2OUT |= 0x07;
  }
  else
  {
    P2OUT &= ~0x07;
  }

  // Clear interrupt flag
  TA0IV &= ~TA0IV_TAIFG;

}
