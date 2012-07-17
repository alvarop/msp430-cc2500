/*
 * main.c
 */
#include "device.h"
#include <stdint.h>
#include "cc2500.h"

#define RGB_MIN (40)

uint8_t cc2500_rx_callback( uint8_t*, uint8_t );

#define STROBE_PIN BIT2
#define RESET_PIN BIT1

#define TOTAL_SAMPLES (7)

volatile uint8_t samples[TOTAL_SAMPLES];
volatile uint8_t sample_index = 0;

volatile uint8_t new_data = 0;

uint8_t txBuffer[6];
uint8_t rgb[3];

void main(void) {

  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  // Setup oscillator for 16MHz operation
  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL = CALDCO_16MHZ;

  // Wait for changes to take effect
  __delay_cycles(4000);

  // Setup CC2500 radio and register callback to process incoming packets
  setup_cc2500(cc2500_rx_callback);

  P1OUT = 0x00;
  P1DIR |= (STROBE_PIN+RESET_PIN + BIT3);
  P1SEL |= (STROBE_PIN);

  // Pulse the reset pin for 1uS
  P1OUT |= RESET_PIN;
  __delay_cycles(16);
  P1OUT &= ~RESET_PIN;

  ADC10CTL1 = INCH_0;   // P1.0
  ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE;

  // Setup timer A

  TACCTL1 = OUTMOD_7;
  TA0CCR0 = 9524; // 9524/2MHz = 4.762 ms strobe period
  TACCR1 = 524;   // 262uS strobe pulse

  TA0CCR2 = (TA0CCR0>>1); // Trigger ADC measurement halfway through the period
  TACCTL2 = CCIE;

  // SMCLK, up mode
  TA0CTL = TASSEL_2 + MC_1 + ID_3 + TAIE + TACLR;

  txBuffer[0] = 4;         // Packet length
  txBuffer[1] = 0x01;      // Packet address

  for(;;) {
    __bis_SR_register( LPM1_bits + GIE );   // Enable interrupts and sleep
    if(new_data)
    {


      rgb[0] = samples[0] + samples[1];
      rgb[1] = samples[2] + samples[3];
      rgb[2] = samples[4] + samples[5];

      if(rgb[0] < RGB_MIN ) {
        rgb[0]=0;
      }
      if(rgb[1] < RGB_MIN ) {
        rgb[1]=0;
      }
      if(rgb[2] < RGB_MIN ) {
        rgb[2]=0;
      }

      txBuffer[2] = rgb[0];       // red
      txBuffer[3] = rgb[1];       // green
      txBuffer[4] = rgb[2];       // blue

      // Send out packet with RGB values
      cc2500_tx(txBuffer, 5);
      new_data = 0;


    }
  }
}

// This function is called to process the received packet
uint8_t cc2500_rx_callback( uint8_t* buffer, uint8_t length )
{
  // Currently not used
  return 0;
}

// Timer A isr
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TA1_ISR (void)
{
  // Start ADC conversion
  ADC10CTL0 |= ENC + ADC10SC;

  // Clear interrupt flag
  TA0IV &= ~TA0IV_TAIFG;

}

// ADC10 isr
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {

  // Store sample
  samples[sample_index++] = (ADC10MEM >> 2) & 0xFF;

  if(sample_index == TOTAL_SAMPLES)
  {
    // Send message?
    sample_index = 0;

    new_data = 1;

    // Pulse the reset pin for 1uS
    P1OUT |= RESET_PIN;
    __delay_cycles(16);
    P1OUT &= ~RESET_PIN;

    // Wakeup
    __bic_SR_register_on_exit(LPM1_bits);
  }

}
