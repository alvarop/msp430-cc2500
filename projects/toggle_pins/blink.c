
#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;             // Stop watchdog timer
  P1DIR |= 0xFF;                        // Set P1.0 to output direction
  P2DIR |= 0xFF;                        // Set P1.0 to output direction

  for (;;)
  {
	volatile unsigned int i;            // volatile to prevent optimization

	P1OUT ^= 0xFF;
	P2OUT ^= 0xFF;

	i = 10000;                          // SW Delay
	do i--;
	while (i != 0);
  }
}
