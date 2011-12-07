//------------------------------------------------------------------------------
//  Description:  This file contains functions that allow the MSP430 device to
//  access the SPI interface of the CC1100/CC2500.  There are multiple
//  instances of each function; the one to be compiled is selected by the
//  system variable TI_CC_RF_SER_INTF, defined in "TI_CC_hardware_board.h".
//
//  MSP430/CC1100-2500 Interface Code Library v1.1
//
//  W. Goh
//  Texas Instruments, Inc.
//  December 2009
//  IAR Embedded Workbench v4.20
//------------------------------------------------------------------------------
// Change Log:
//------------------------------------------------------------------------------
// Version:  1.1
// Comments: Fixed several bugs where it is stuck in a infinite while loop
//           Added support for 5xx
//
// Version:  1.00
// Comments: Initial Release Version
//------------------------------------------------------------------------------

#include "TI_CC_include.h"
#include "TI_CC_spi.h"

//------------------------------------------------------------------------------
//  void TI_CC_SPISetup(void)
//
//  DESCRIPTION:
//  Configures the assigned interface to function as a SPI port and
//  initializes it.
//------------------------------------------------------------------------------
//  void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
//
//  DESCRIPTION:
//  Writes "value" to a single configuration register at address "addr".
//------------------------------------------------------------------------------
//  void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
//
//  DESCRIPTION:
//  Writes values to multiple configuration registers, the first register being
//  at address "addr".  First data byte is at "buffer", and both addr and
//  buffer are incremented sequentially (within the CCxxxx and MSP430,
//  respectively) until "count" writes have been performed.
//------------------------------------------------------------------------------
//  uint8_t TI_CC_SPIReadReg(uint8_t addr)
//
//  DESCRIPTION:
//  Reads a single configuration register at address "addr" and returns the
//  value read.
//------------------------------------------------------------------------------
//  void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
//
//  DESCRIPTION:
//  Reads multiple configuration registers, the first register being at address
//  "addr".  Values read are deposited sequentially starting at address
//  "buffer", until "count" registers have been read.
//------------------------------------------------------------------------------
//  uint8_t TI_CC_SPIReadStatus(uint8_t addr)
//
//  DESCRIPTION:
//  Special read function for reading status registers.  Reads status register
//  at register "addr" and returns the value read.
//------------------------------------------------------------------------------
//  void TI_CC_SPIStrobe(uint8_t strobe)
//
//  DESCRIPTION:
//  Special write function for writing to command strobe registers.  Writes
//  to the strobe at address "addr".
//------------------------------------------------------------------------------


// Delay function. # of CPU cycles delayed is similar to "cycles". Specifically,
// it's ((cycles-15) % 6) + 15.  Not exact, but gives a sense of the real-time
// delay.  Also, if MCLK ~1MHz, "cycles" is similar to # of useconds delayed.
void TI_CC_Wait(uint16_t cycles)
{
  while(cycles>15)                          // 15 cycles consumed by overhead
    cycles = cycles - 6;                    // 6 cycles consumed each iteration
}

//******************************************************************************
// If USART0 is used
//******************************************************************************
#if TI_CC_RF_SER_INTF == TI_CC_SER_INTF_USART0

void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  ME1 |= USPIE0;                            // Enable USART0 SPI mode
  UCTL0 = SWRST;                            // Disable USART state machine
  UCTL0 |= CHAR + SYNC + MM;                // 8-bit SPI Master **SWRST**
  UTCTL0 |= CKPH + SSEL1 + SSEL0 + STC;     // SMCLK, 3-pin mode
  UBR00 = 0x02;                             // UCLK/2
  UBR10 = 0x00;                             // 0
  UMCTL0 = 0x00;                            // No modulation
  TI_CC_SPI_USART0_PxSEL |= TI_CC_SPI_USART0_SIMO
                          | TI_CC_SPI_USART0_SOMI
                          | TI_CC_SPI_USART0_UCLK;
                                            // SPI option select
  TI_CC_SPI_USART0_PxDIR |= TI_CC_SPI_USART0_SIMO + TI_CC_SPI_USART0_UCLK;
                                            // SPI TX out direction
  UCTL0 &= ~SWRST;                          // Initialize USART state machine
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG1&UTXIFG0));                  // Wait for TX to finish
  U0TXBUF = addr;                           // Send address
  while (!(IFG1&UTXIFG0));                  // Wait for TX to finish
  U0TXBUF = value;                          // Send value
  while(!(UTCTL0&TXEPT));                   // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG1 & UTXIFG0));                // Wait for TX to finish
  U0TXBUF = addr | TI_CCxxx0_WRITE_BURST;   // Send address
  for (i = 0; i < count; i++)
  {
    while (!(IFG1 & UTXIFG0));              // Wait for TX to finish
    U0TXBUF = buffer[i];                    // Send data
  }
  while(!(UTCTL0 & TXEPT));
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG1 & UTXIFG0));                // Wait for TX to finish
  U0TXBUF = (addr | TI_CCxxx0_READ_SINGLE); // Send address
  while (!(IFG1 & UTXIFG0));                // Wait for TX to finish
  U0TXBUF = 0;                              // Dummy write so we can read data
  while(!(UTCTL0 & TXEPT));                 // Wait for TX complete
  x = U0RXBUF;                              // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG1 & UTXIFG0));                // Wait for TXBUF ready
  U0TXBUF = (addr | TI_CCxxx0_READ_BURST);  // Send address
  while(!(UTCTL0 & TXEPT));                 // Wait for TX complete
  U0TXBUF = 0;                              // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  IFG1 &= ~URXIFG0;                         // Clear flag
  while (!(IFG1&URXIFG0));                  // Wait for end of 1st data byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    U0TXBUF = 0;                            // Initiate next data RX, meanwhile
    buffer[i] = U0RXBUF;                    // Store data from last data RX
    while (!(IFG1&URXIFG0));                // Wait for end of data RX
  }
  buffer[count-1] = U0RXBUF;                // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

// For status/strobe addresses, the BURST bit selects between status registers
// and command strobes.
uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG1 & UTXIFG0));                // Wait for TX to finish
  U0TXBUF = (addr | TI_CCxxx0_READ_BURST);  // Send address
  while (!(IFG1 & UTXIFG0));                // Wait for TX to finish
  U0TXBUF = 0;                              // Dummy write so we can read data
  while(!(UTCTL0 & TXEPT));                 // Wait for TX complete
  status = U0RXBUF;                         // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG1 & UTXIFG0));                // Wait for TX to finish
  U0TXBUF = strobe;                         // Send strobe
  // Strobe addr is now being TX'ed
  IFG1 &= ~URXIFG0;                         // Clear flag
  while(!(UTCTL0 & TXEPT));                 // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG1 & UTXIFG0));                // Wait for TX to finish
  U0TXBUF = TI_CCxxx0_SRES;                 // Send strobe
  // Strobe addr is now being TX'ed
  while(!(UTCTL0 & TXEPT));                 // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

// END USART0
//******************************************************************************
// If USART1 is used
//******************************************************************************
#elif TI_CC_RF_SER_INTF == TI_CC_SER_INTF_USART1

void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  ME2 |= USPIE1;                            // Enable USART1 SPI mode
  UCTL1 = SWRST;                            // Disable USART state machine
  UCTL1 |= CHAR + SYNC + MM;                // 8-bit SPI Master **SWRST**
  UTCTL1 |= CKPH + SSEL1 + SSEL0 + STC;     // SMCLK, 3-pin mode
  UBR01 = 0x02;                             // UCLK/2
  UBR11 = 0x00;                             // 0
  UMCTL1 = 0x00;                            // No modulation
  TI_CC_SPI_USART1_PxSEL |= TI_CC_SPI_USART1_SIMO
                          | TI_CC_SPI_USART1_SOMI
                          | TI_CC_SPI_USART1_UCLK;
                                            // SPI option select
  TI_CC_SPI_USART1_PxDIR |= TI_CC_SPI_USART1_SIMO + TI_CC_SPI_USART1_UCLK;
                                            // SPI TXD out direction
  UCTL1 &= ~SWRST;                          // Initialize USART state machine
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UTXIFG1));                  // Wait for TX to finish
  U1TXBUF = addr;                           // Send address
  while (!(IFG2&UTXIFG1));                  // Wait for TX to finish
  U1TXBUF = value;                          // Load data for TX after addr
  while(!(UTCTL1&TXEPT));                   // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UTXIFG1));                  // Wait for TX to finish
  U1TXBUF = addr | TI_CCxxx0_WRITE_BURST;   // Send address
  for (i = 0; i < count; i++)
  {
    while (!(IFG2&UTXIFG1));                // Wait for TX to finish
    U1TXBUF = buffer[i];                    // Send data
  }
  while(!(UTCTL1&TXEPT));                   // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UTXIFG1));                  // Wait for TX to finish
  U1TXBUF = (addr | TI_CCxxx0_READ_SINGLE); // Send address
  while (!(IFG2&UTXIFG1));                  // Wait for TX to finish
  U1TXBUF = 0;                              // Load dummy byte for TX after addr
  while(!(UTCTL1&TXEPT));                   // Wait for TX complete
  x = U1RXBUF;                              // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UTXIFG1));                  // Wait for TXBUF ready
  U1TXBUF = (addr | TI_CCxxx0_READ_BURST);  // Send address
  while(!(UTCTL1&TXEPT));                   // Wait for TX complete
  U1TXBUF = 0;                              // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  IFG2 &= ~URXIFG1;                         // Clear flag
  while (!(IFG2&URXIFG1));                  // Wait for end of 1st data byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    U1TXBUF = 0;                            // Initiate next data RX, meanwhile
    buffer[i] = U1RXBUF;                    // Store data from last data RX
    while (!(IFG2&URXIFG1));                // Wait for end of data RX
  }
  buffer[count-1] = U1RXBUF;                // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UTXIFG1));                  // Wait for TX to finish
  U1TXBUF = (addr | TI_CCxxx0_READ_BURST);  // Send address
  while (!(IFG2&UTXIFG1));                  // Wait for TX to finish
  U1TXBUF = 0;                              // Dummy write so we can read data
  while(!(UTCTL1&TXEPT));                   // Wait for TX complete
  status = U1RXBUF;                         // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UTXIFG1));                  // Wait for TX to finish
  U1TXBUF = strobe;                         // Send strobe
  // Strobe addr is now being TX'ed
  while(!(UTCTL1&TXEPT));                   // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2 & UTXIFG1));                // Wait for TX to finish
  U1TXBUF = TI_CCxxx0_SRES;                 // Send strobe
  // Strobe addr is now being TX'ed
  while(!(UTCTL1&TXEPT));                   // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

// END TI_CC_SER_INTF_USART1
//******************************************************************************
// If USCIA0 is used
//   |-- If 5xx
//         |-- Use 5xx Init
//   |-- Else
//         |-- Use 2xx, 4xx Init
//******************************************************************************
#elif TI_CC_RF_SER_INTF == TI_CC_SER_INTF_USCIA0

//******************************************************************************
// Support for 5xx USCI_A0
//******************************************************************************
#ifdef TI_5xx
void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  UCA0CTL1 |= UCSWRST;                      // **Disable USCI state machine**
  UCA0CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 0x02;                           // UCLK/2
  UCA0BR1 = 0;
  UCA0MCTL = 0;
  TI_CC_SPI_USCIA0_PxSEL |= TI_CC_SPI_USCIA0_SIMO
                         | TI_CC_SPI_USCIA0_SOMI
                         | TI_CC_SPI_USCIA0_UCLK;
                                            // SPI option select
  TI_CC_SPI_USCIA0_PxDIR |= TI_CC_SPI_USCIA0_SIMO | TI_CC_SPI_USCIA0_UCLK;
                                            // SPI TXD out direction
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA0TXBUF = addr;                         // Send address
  while (!(UCA0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA0TXBUF = value;                        // Send data
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA0TXBUF = addr | TI_CCxxx0_WRITE_BURST; // Send address
  for (i = 0; i < count; i++)
  {
    while (!(UCA0IFG&UCTXIFG));               // Wait for TXBUF ready
    UCA0TXBUF = buffer[i];                  // Send data
  }
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA0TXBUF = (addr | TI_CCxxx0_READ_SINGLE);// Send address
  while (!(UCA0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA0TXBUF = 0;                            // Dummy write so we can read data
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  x = UCA0RXBUF;                            // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA0TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  UCA0TXBUF = 0;                            // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  UCA0IFG &= ~UCRXIFG;                      // Clear flag
  while (!(UCA0IFG&UCRXIFG));               // Wait for end of addr byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    UCA0TXBUF = 0;                          //Initiate next data RX, meanwhile..
    buffer[i] = UCA0RXBUF;                  // Store data from last data RX
    while (!(UCA0IFG&UCRXIFG));             // Wait for RX to finish
  }
  buffer[count-1] = UCA0RXBUF;              // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA0TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (!(UCA0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA0TXBUF = 0;                            // Dummy write so we can read data
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  status = UCA0RXBUF;                       // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA0TXBUF = strobe;                       // Send strobe
  // Strobe addr is now being TX'ed
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA0TXBUF = TI_CCxxx0_SRES;               // Send strobe
  // Strobe addr is now being TX'ed
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

// End of support for 5xx USCI_A0

#else
void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  UCA0CTL1 |= UCSWRST;                      // **Disable USCI state machine**
  UCA0CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 0x02;                           // UCLK/2
  UCA0BR1 = 0;
  UCA0MCTL = 0;
  TI_CC_SPI_USCIA0_PxSEL |= TI_CC_SPI_USCIA0_SIMO
                         | TI_CC_SPI_USCIA0_SOMI
                         | TI_CC_SPI_USCIA0_UCLK;
                                            // SPI option select
  TI_CC_SPI_USCIA0_PxDIR |= TI_CC_SPI_USCIA0_SIMO | TI_CC_SPI_USCIA0_UCLK;
                                            // SPI TXD out direction
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCA0TXIFG));                // Wait for TXBUF ready
  UCA0TXBUF = addr;                         // Send address
  while (!(IFG2&UCA0TXIFG));                // Wait for TXBUF ready
  UCA0TXBUF = value;                        // Send data
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCA0TXIFG));                // Wait for TXBUF ready
  UCA0TXBUF = addr | TI_CCxxx0_WRITE_BURST; // Send address
  for (i = 0; i < count; i++)
  {
    while (!(IFG2&UCA0TXIFG));              // Wait for TXBUF ready
    UCA0TXBUF = buffer[i];                  // Send data
  }
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCA0TXIFG));                // Wait for TX to finish
  UCA0TXBUF = (addr | TI_CCxxx0_READ_SINGLE);// Send address
  while (!(IFG2&UCA0TXIFG));                // Wait for TX to finish
  UCA0TXBUF = 0;                            // Dummy write so we can read data
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  x = UCA0RXBUF;                            // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCA0TXIFG));                // Wait for TX to finish
  UCA0TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  UCA0TXBUF = 0;                            // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  IFG2 &= ~UCA0RXIFG;                       // Clear flag
  while (!(IFG2&UCA0RXIFG));                // Wait for end of addr byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    UCA0TXBUF = 0;                          //Initiate next data RX, meanwhile..
    buffer[i] = UCA0RXBUF;                  // Store data from last data RX
    while (!(IFG2&UCA0RXIFG));              // Wait for RX to finish
  }
  buffer[count-1] = UCA0RXBUF;              // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCA0TXIFG));                // Wait for TX to finish
  UCA0TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (!(IFG2&UCA0TXIFG));                // Wait for TX to finish
  UCA0TXBUF = 0;                            // Dummy write so we can read data
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  status = UCA0RXBUF;                       // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCA0TXIFG));                // Wait for TX to finish
  UCA0TXBUF = strobe;                       // Send strobe
  // Strobe addr is now being TX'ed
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCA0TXIFG));                // Wait for TXBUF ready
  UCA0TXBUF = TI_CCxxx0_SRES;               // Send strobe
  // Strobe addr is now being TX'ed
  while (UCA0STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

#endif
// END TI_CC_SER_INTF_USCIA0

//******************************************************************************
// If USCIA1 is used
//******************************************************************************
#elif TI_CC_RF_SER_INTF == TI_CC_SER_INTF_USCIA1

//******************************************************************************
// Support for 5xx USCI_A1
//******************************************************************************
#ifdef TI_5xx
void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  UCA1CTL1 |= UCSWRST;                      // **Disable USCI state machine**
  UCA1CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
  UCA1CTL1 |= UCSSEL_2;                     // SMCLK
  UCA1BR0 = 0x02;                           // UCLK/2
  UCA1BR1 = 0;
  UCA1MCTL = 0;
  TI_CC_SPI_USCIA1_PxSEL |= TI_CC_SPI_USCIA1_SIMO
                         | TI_CC_SPI_USCIA1_SOMI;
  TI_CC_SPI_USCIA1_PxSEL_UCLK |= TI_CC_SPI_USCIA1_UCLK;
                                            // SPI option select
  TI_CC_SPI_USCIA1_PxDIR |= TI_CC_SPI_USCIA1_SIMO;
  TI_CC_SPI_USCIA1_PxDIR_UCLK |=  TI_CC_SPI_USCIA1_UCLK;
                                            // SPI TXD out direction
  UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA1TXBUF = addr;                         // Send address
  while (!(UCA1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA1TXBUF = value;                        // Send data
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA1TXBUF = addr | TI_CCxxx0_WRITE_BURST; // Send address
  for (i = 0; i < count; i++)
  {
    while (!(UCA1IFG&UCTXIFG));               // Wait for TXBUF ready
    UCA1TXBUF = buffer[i];                  // Send data
  }
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA1TXBUF = (addr | TI_CCxxx0_READ_SINGLE);// Send address
  while (!(UCA1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA1TXBUF = 0;                            // Dummy write so we can read data
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  x = UCA1RXBUF;                            // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA1TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  UCA1TXBUF = 0;                            // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  UCA1IFG &= ~UCRXIFG;                      // Clear flag
  while (!(UCA1IFG&UCRXIFG));               // Wait for end of addr byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    UCA1TXBUF = 0;                          //Initiate next data RX, meanwhile..
    buffer[i] = UCA1RXBUF;                  // Store data from last data RX
    while (!(UCA1IFG&UCRXIFG));             // Wait for RX to finish
  }
  buffer[count-1] = UCA1RXBUF;              // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA1TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (!(UCA1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA1TXBUF = 0;                            // Dummy write so we can read data
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  status = UCA1RXBUF;                       // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA1TXBUF = strobe;                       // Send strobe
  // Strobe addr is now being TX'ed
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA1TXBUF = TI_CCxxx0_SRES;               // Send strobe
  // Strobe addr is now being TX'ed
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

// End of support for 5xx USCI_A1

#else
void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  UCA1CTL1 |= UCSWRST;                      // **Disable USCI state machine**
  UCA1CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
  UCA1CTL1 |= UCSSEL_2;                     // SMCLK
  UCA1BR0 = 0x02;                           // UCLK/2
  UCA1BR1 = 0;
  UCA1MCTL = 0;
  TI_CC_SPI_USCIA1_PxSEL |= TI_CC_SPI_USCIA1_SIMO
                         | TI_CC_SPI_USCIA1_SOMI
                         | TI_CC_SPI_USCIA1_UCLK;
                                            // SPI option select
  TI_CC_SPI_USCIA1_PxDIR |= TI_CC_SPI_USCIA1_SIMO | TI_CC_SPI_USCIA1_UCLK;
                                            // SPI TXD out direction
  UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCA1TXIFG));              // Wait for TXBUF ready
  UCA1TXBUF = addr;                         // Send address
  while (!(UC1IFG&UCA1TXIFG));              // Wait for TXBUF ready
  UCA1TXBUF = value;                        // Send data
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCA1TXIFG));              // Wait for TXBUF ready
  UCA1TXBUF = addr | TI_CCxxx0_WRITE_BURST; // Send address
  for (i = 0; i < count; i++)
  {
    while (!(UC1IFG&UCA1TXIFG));            // Wait for TXBUF ready
    UCA1TXBUF = buffer[i];                  // Send data
  }
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCA1TXIFG));              // Wait for TXBUF ready
  UCA1TXBUF = (addr | TI_CCxxx0_READ_SINGLE);// Send address
  while (!(UC1IFG&UCA1TXIFG));              // Wait for TX to finish
  UCA1TXBUF = 0;                            // Dummy write so we can read data
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  x = UCA1RXBUF;                            // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCA1TXIFG));              // Wait for TXBUF ready
  UCA1TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  UCA1TXBUF = 0;                            // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  UC1IFG &= ~UCA1RXIFG;                     // Clear flag
  while (!(UC1IFG&UCA1RXIFG));              // Wait for end of 1st data byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    UCA1TXBUF = 0;                          //Initiate next data RX, meanwhile..
    buffer[i] = UCA1RXBUF;                  // Store data from last data RX
    while (!(UC1IFG&UCA1RXIFG));            // Wait for RX to finish
  }
  buffer[count-1] = UCA1RXBUF;              // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCA1TXIFG));              // Wait for TXBUF ready
  UCA1TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (!(UC1IFG&UCA1TXIFG));              // Wait for TXBUF ready
  UCA1TXBUF = 0;                            // Dummy write so we can read data
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  status = UCA1RXBUF;                       // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCA1TXIFG));              // Wait for TXBUF ready
  UCA1TXBUF = strobe;                       // Send strobe
  // Strobe addr is now being TX'ed
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCA1TXIFG));              // Wait for TXBUF ready
  UCA1TXBUF = TI_CCxxx0_SRES;               // Send strobe
  // Strobe addr is now being TX'ed
  while (UCA1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

#endif
// End of USCIA1

//******************************************************************************
// If USCIA2 is used
//   |-- If 5xx
//         |-- Use 5xx Init
//******************************************************************************
#elif TI_CC_RF_SER_INTF == TI_CC_SER_INTF_USCIA2

//******************************************************************************
// Support for 5xx USCI_A2
//******************************************************************************
#ifdef TI_5xx
void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  UCA2CTL1 |= UCSWRST;                      // **Disable USCI state machine**
  UCA2CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
  UCA2CTL1 |= UCSSEL_2;                     // SMCLK
  UCA2BR0 = 0x02;                           // UCLK/2
  UCA2BR1 = 0;
  TI_CC_SPI_USCIA2_PxSEL |= TI_CC_SPI_USCIA2_SOMI
                         | TI_CC_SPI_USCIA2_UCLK
                         | TI_CC_SPI_USCIA2_SIMO;
                                            // SPI option select
  TI_CC_SPI_USCIA2_PxDIR |= TI_CC_SPI_USCIA2_UCLK | TI_CC_SPI_USCIA2_SIMO;
                                            // SPI TXD out direction
  UCA2CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA2TXBUF = addr;                         // Send address
  while (!(UCA2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA2TXBUF = value;                        // Send data
  while (UCA2STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA2TXBUF = addr | TI_CCxxx0_WRITE_BURST; // Send address
  for (i = 0; i < count; i++)
  {
    while (!(UCA2IFG&UCTXIFG));             // Wait for TXBUF ready
    UCA2TXBUF = buffer[i];                  // Send data
  }
  while (UCA2STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA2TXBUF = (addr | TI_CCxxx0_READ_SINGLE);// Send address
  while (!(UCA2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA2TXBUF = 0;                            // Dummy write so we can read data
  while (UCA2STAT & UCBUSY);                // Wait for TX complete
  x = UCA2RXBUF;                            // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA2TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (UCA2STAT & UCBUSY);                // Wait for TX complete
  UCA2TXBUF = 0;                            // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  UCA2IFG &= ~UCRXIFG;                      // Clear flag
  while (!(UCA2IFG&UCRXIFG));               // Wait for end of addr byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    UCA2TXBUF = 0;                          //Initiate next data RX, meanwhile..
    buffer[i] = UCA2RXBUF;                  // Store data from last data RX
    while (!(UCA2IFG&UCRXIFG));             // Wait for RX to finish
  }
  buffer[count-1] = UCA2RXBUF;              // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA2TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (!(UCA2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA2TXBUF = 0;                            // Dummy write so we can read data
  while (UCA2STAT & UCBUSY);                // Wait for TX complete
  status = UCA2RXBUF;                       // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA2TXBUF = strobe;                       // Send strobe
  // Strobe addr is now being TX'ed
  while (UCA2STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA2TXBUF = TI_CCxxx0_SRES;               // Send strobe
  // Strobe addr is now being TX'ed
  while (UCA2STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

// End of support for 5xx USCI_A2
#endif
// End of support for USCI_A2

//******************************************************************************
// If USCIA3 is used
//   |-- If 5xx
//         |-- Use 5xx Init
//******************************************************************************
#elif TI_CC_RF_SER_INTF == TI_CC_SER_INTF_USCIA3

//******************************************************************************
// Support for 5xx USCI_A3
//******************************************************************************
#ifdef TI_5xx
void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  UCA3CTL1 |= UCSWRST;                      // **Disable USCI state machine**
  UCA3CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
  UCA3CTL1 |= UCSSEL_2;                     // SMCLK
  UCA3BR0 = 0x02;                           // UCLK/2
  UCA3BR1 = 0;
  TI_CC_SPI_USCIA3_PxSEL |= TI_CC_SPI_USCIA3_SOMI
                         | TI_CC_SPI_USCIA3_UCLK
                         | TI_CC_SPI_USCIA3_SIMO;
                                            // SPI option select
  TI_CC_SPI_USCIA3_PxDIR |= TI_CC_SPI_USCIA3_UCLK | TI_CC_SPI_USCIA3_SIMO;
                                            // SPI TXD out direction
  UCA3CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA3TXBUF = addr;                         // Send address
  while (!(UCA3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA3TXBUF = value;                        // Send data
  while (UCA3STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA3TXBUF = addr | TI_CCxxx0_WRITE_BURST; // Send address
  for (i = 0; i < count; i++)
  {
    while (!(UCA3IFG&UCTXIFG));             // Wait for TXBUF ready
    UCA3TXBUF = buffer[i];                  // Send data
  }
  while (UCA3STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA3TXBUF = (addr | TI_CCxxx0_READ_SINGLE);// Send address
  while (!(UCA3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA3TXBUF = 0;                            // Dummy write so we can read data
  while (UCA3STAT & UCBUSY);                // Wait for TX complete
  x = UCA3RXBUF;                            // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA3TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (UCA3STAT & UCBUSY);                // Wait for TX complete
  UCA3TXBUF = 0;                            // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  UCA3IFG &= ~UCRXIFG;                      // Clear flag
  while (!(UCA3IFG&UCRXIFG));               // Wait for end of addr byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    UCA3TXBUF = 0;                          //Initiate next data RX, meanwhile..
    buffer[i] = UCA3RXBUF;                  // Store data from last data RX
    while (!(UCA3IFG&UCRXIFG));             // Wait for RX to finish
  }
  buffer[count-1] = UCA3RXBUF;              // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA3TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (!(UCA3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA3TXBUF = 0;                            // Dummy write so we can read data
  while (UCA3STAT & UCBUSY);                // Wait for TX complete
  status = UCA3RXBUF;                       // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA3TXBUF = strobe;                       // Send strobe
  // Strobe addr is now being TX'ed
  while (UCA3STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCA3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCA3TXBUF = TI_CCxxx0_SRES;               // Send strobe
  // Strobe addr is now being TX'ed
  while (UCA3STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

// End of support for 5xx USCI_A3
#endif
// End of support for USCI_A3

//******************************************************************************
// If USCIB0 is used
//   |-- If 5xx
//         |-- Use 5xx Init
//   |-- Else
//         |-- Use 2xx, 4xx Init
//******************************************************************************
#elif TI_CC_RF_SER_INTF == TI_CC_SER_INTF_USCIB0

//******************************************************************************
// Support for 5xx USCI_B0
//******************************************************************************
#ifdef TI_5xx
void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  UCB0CTL1 |= UCSWRST;                      // **Disable USCI state machine**
  UCB0CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
  UCB0CTL1 |= UCSSEL_2;                     // SMCLK
  UCB0BR0 = 0x010;                           // UCLK/16
  UCB0BR1 = 0;
  TI_CC_SPI_USCIB0_PxSEL |= TI_CC_SPI_USCIB0_SIMO
                         | TI_CC_SPI_USCIB0_SOMI
                         | TI_CC_SPI_USCIB0_UCLK;
                                            // SPI option select
  TI_CC_SPI_USCIB0_PxDIR |= TI_CC_SPI_USCIB0_SIMO | TI_CC_SPI_USCIB0_UCLK;
                                            // SPI TXD out direction
  UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB0TXBUF = addr;                         // Send address
  while (!(UCB0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB0TXBUF = value;                        // Send data
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB0TXBUF = addr | TI_CCxxx0_WRITE_BURST; // Send address
  for (i = 0; i < count; i++)
  {
    while (!(UCB0IFG&UCTXIFG));               // Wait for TXBUF ready
    UCB0TXBUF = buffer[i];                  // Send data
  }
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB0TXBUF = (addr | TI_CCxxx0_READ_SINGLE);// Send address
  while (!(UCB0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB0TXBUF = 0;                            // Dummy write so we can read data
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  x = UCB0RXBUF;                            // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB0TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  UCB0TXBUF = 0;                            // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  UCB0IFG &= ~UCRXIFG;                      // Clear flag
  while (!(UCB0IFG&UCRXIFG));               // Wait for end of 1st data byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    UCB0TXBUF = 0;                          //Initiate next data RX, meanwhile..
    buffer[i] = UCB0RXBUF;                  // Store data from last data RX
    while (!(UCB0IFG&UCRXIFG));              // Wait for RX to finish
  }
  buffer[count-1] = UCB0RXBUF;              // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB0TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (!(UCB0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB0TXBUF = 0;                            // Dummy write so we can read data
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  status = UCB0RXBUF;                       // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB0TXBUF = strobe;                       // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB0IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB0TXBUF = TI_CCxxx0_SRES;               // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

#else
void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  UCB0CTL1 |= UCSWRST;                      // **Disable USCI state machine**
  UCB0CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
  UCB0CTL1 |= UCSSEL_2;                     // SMCLK
  UCB0BR0 = 0x02;                           // UCLK/2
  UCB0BR1 = 0;
  TI_CC_SPI_USCIB0_PxSEL |= TI_CC_SPI_USCIB0_SIMO
                         | TI_CC_SPI_USCIB0_SOMI
                         | TI_CC_SPI_USCIB0_UCLK;
                                            // SPI option select
  TI_CC_SPI_USCIB0_PxDIR |= TI_CC_SPI_USCIB0_SIMO | TI_CC_SPI_USCIB0_UCLK;
                                            // SPI TXD out direction
  UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = addr;                         // Send address
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = value;                        // Send data
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = addr | TI_CCxxx0_WRITE_BURST; // Send address
  for (i = 0; i < count; i++)
  {
    while (!(IFG2&UCB0TXIFG));              // Wait for TXBUF ready
    UCB0TXBUF = buffer[i];                  // Send data
  }
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = (addr | TI_CCxxx0_READ_SINGLE);// Send address
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = 0;                            // Dummy write so we can read data
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  x = UCB0RXBUF;                            // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  UCB0TXBUF = 0;                            // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  IFG2 &= ~UCB0RXIFG;                       // Clear flag
  while (!(IFG2&UCB0RXIFG));                // Wait for end of 1st data byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    UCB0TXBUF = 0;                          //Initiate next data RX, meanwhile..
    buffer[i] = UCB0RXBUF;                  // Store data from last data RX
    while (!(IFG2&UCB0RXIFG));              // Wait for RX to finish
  }
  buffer[count-1] = UCB0RXBUF;              // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = 0;                            // Dummy write so we can read data
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  status = UCB0RXBUF;                       // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = strobe;                       // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = TI_CCxxx0_SRES;               // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

#endif
// End of USCIB0

//******************************************************************************
// If USCIB1 is used
//******************************************************************************
#elif TI_CC_RF_SER_INTF == TI_CC_SER_INTF_USCIB1

//******************************************************************************
// Support for 5xx USCI_B1
//******************************************************************************
#ifdef TI_5xx
void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  UCB1CTL1 |= UCSWRST;                      // **Disable USCI state machine**
  UCB1CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
  UCB1CTL1 |= UCSSEL_2;                     // SMCLK
  UCB1BR0 = 0x02;                           // UCLK/2
  UCB1BR1 = 0;
  TI_CC_SPI_USCIB1_PxSEL |= TI_CC_SPI_USCIB1_SOMI
                         | TI_CC_SPI_USCIB1_UCLK;
  TI_CC_SPI_USCIB1_PxSEL_SIMO |= TI_CC_SPI_USCIB1_SIMO;
                                            // SPI option select
  TI_CC_SPI_USCIB1_PxDIR |= TI_CC_SPI_USCIB1_UCLK;
  TI_CC_SPI_USCIB1_PxDIR_SIMO |=  TI_CC_SPI_USCIB1_SIMO;
                                            // SPI TXD out direction
  UCB1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB1TXBUF = addr;                         // Send address
  while (!(UCB1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB1TXBUF = value;                        // Send data
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB1TXBUF = addr | TI_CCxxx0_WRITE_BURST; // Send address
  for (i = 0; i < count; i++)
  {
    while (!(UCB1IFG&UCTXIFG));             // Wait for TXBUF ready
    UCB1TXBUF = buffer[i];                  // Send data
  }
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB1TXBUF = (addr | TI_CCxxx0_READ_SINGLE);// Send address
  while (!(UCB1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB1TXBUF = 0;                            // Dummy write so we can read data
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  x = UCB1RXBUF;                            // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB1TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  UCB1TXBUF = 0;                            // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  UCB1IFG &= ~UCRXIFG;                      // Clear flag
  while (!(UCB1IFG&UCRXIFG));               // Wait for end of addr byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    UCB1TXBUF = 0;                          //Initiate next data RX, meanwhile..
    buffer[i] = UCB1RXBUF;                  // Store data from last data RX
    while (!(UCB1IFG&UCRXIFG));             // Wait for RX to finish
  }
  buffer[count-1] = UCB1RXBUF;              // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB1TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (!(UCB1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB1TXBUF = 0;                            // Dummy write so we can read data
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  status = UCB1RXBUF;                       // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB1TXBUF = strobe;                       // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB1IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB1TXBUF = TI_CCxxx0_SRES;               // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

// End of support for 5xx USCI_B1
#else

void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  UCB1CTL1 |= UCSWRST;                      // **Disable USCI state machine**
  UCB1CTL0 |= UCMST+UCCKPL+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
  UCB1CTL1 |= UCSSEL_2;                     // SMCLK
  UCB1BR0 = 0x02;                           // UCLK/2
  UCB1BR1 = 0;
  TI_CC_SPI_USCIB1_PxSEL |= TI_CC_SPI_USCIB1_SIMO
                         | TI_CC_SPI_USCIB1_SOMI
                         | TI_CC_SPI_USCIB1_UCLK;
                                            // SPI option select
  TI_CC_SPI_USCIB1_PxDIR |= TI_CC_SPI_USCIB1_SIMO | TI_CC_SPI_USCIB1_UCLK;
                                            // SPI TXD out direction
  UCB1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCB1TXIFG));              // Wait for TXBUF ready
  UCB1TXBUF = addr;                         // Send address
  while (!(UC1IFG&UCB1TXIFG));              // Wait for TXBUF ready
  UCB1TXBUF = value;                        // Send data
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCB1TXIFG));              // Wait for TXBUF ready
  UCB1TXBUF = addr | TI_CCxxx0_WRITE_BURST; // Send address
  for (i = 0; i < count; i++)
  {
    while (!(UC1IFG&UCB1TXIFG));            // Wait for TXBUF ready
    UCB1TXBUF = buffer[i];                  // Send data
  }
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCB1TXIFG));              // Wait for TXBUF ready
  UCB1TXBUF = (addr | TI_CCxxx0_READ_SINGLE);// Send address
  while (!(UC1IFG&UCB1TXIFG));              // Wait for TXBUF ready
  UCB1TXBUF = 0;                            // Dummy write so we can read data
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  x = UCB1RXBUF;                            // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCB1TXIFG));              // Wait for TXBUF ready
  UCB1TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  UCB1TXBUF = 0;                            // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  UC1IFG &= ~UCB1RXIFG;                     // Clear flag
  while (!(UC1IFG&UCB1RXIFG));              // Wait for end of 1st data byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    UCB1TXBUF = 0;                          //Initiate next data RX, meanwhile..
    buffer[i] = UCB1RXBUF;                  // Store data from last data RX
    while (!(UC1IFG&UCB1RXIFG));            // Wait for RX to finish
  }
  buffer[count-1] = UCB1RXBUF;              // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCB1TXIFG));              // Wait for TXBUF ready
  UCB1TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (!(UC1IFG&UCB1TXIFG));              // Wait for TXBUF ready
  UCB1TXBUF = 0;                            // Dummy write so we can read data
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  status = UCB1RXBUF;                       // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCB1TXIFG));              // Wait for TXBUF ready
  UCB1TXBUF = strobe;                       // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UC1IFG&UCB1TXIFG));              // Wait for TXBUF ready
  UCB1TXBUF = TI_CCxxx0_SRES;               // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB1STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

// End of USCIB1
#endif

//******************************************************************************
// If USCIB2 is used
//   |-- If 5xx
//         |-- Use 5xx Init
//******************************************************************************
#elif TI_CC_RF_SER_INTF == TI_CC_SER_INTF_USCIB2

//******************************************************************************
// Support for 5xx USCI_B2
//******************************************************************************
#ifdef TI_5xx
void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  UCB2CTL1 |= UCSWRST;                      // **Disable USCI state machine**
  UCB2CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
  UCB2CTL1 |= UCSSEL_2;                     // SMCLK
  UCB2BR0 = 0x02;                           // UCLK/2
  UCB2BR1 = 0;
  TI_CC_SPI_USCIB2_PxSEL |= TI_CC_SPI_USCIB2_SOMI
                         | TI_CC_SPI_USCIB2_UCLK
                         | TI_CC_SPI_USCIB2_SIMO;
                                            // SPI option select
  TI_CC_SPI_USCIB2_PxDIR |= TI_CC_SPI_USCIB2_UCLK | TI_CC_SPI_USCIB2_SIMO;
                                            // SPI TXD out direction
  UCB2CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB2TXBUF = addr;                         // Send address
  while (!(UCB2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB2TXBUF = value;                        // Send data
  while (UCB2STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB2TXBUF = addr | TI_CCxxx0_WRITE_BURST; // Send address
  for (i = 0; i < count; i++)
  {
    while (!(UCB2IFG&UCTXIFG));             // Wait for TXBUF ready
    UCB2TXBUF = buffer[i];                  // Send data
  }
  while (UCB2STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB2TXBUF = (addr | TI_CCxxx0_READ_SINGLE);// Send address
  while (!(UCB2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB2TXBUF = 0;                            // Dummy write so we can read data
  while (UCB2STAT & UCBUSY);                // Wait for TX complete
  x = UCB2RXBUF;                            // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB2TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (UCB2STAT & UCBUSY);                // Wait for TX complete
  UCB2TXBUF = 0;                            // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  UCB2IFG &= ~UCRXIFG;                      // Clear flag
  while (!(UCB2IFG&UCRXIFG));               // Wait for end of addr byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    UCB2TXBUF = 0;                          //Initiate next data RX, meanwhile..
    buffer[i] = UCB2RXBUF;                  // Store data from last data RX
    while (!(UCB2IFG&UCRXIFG));             // Wait for RX to finish
  }
  buffer[count-1] = UCB2RXBUF;              // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB2TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (!(UCB2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB2TXBUF = 0;                            // Dummy write so we can read data
  while (UCB2STAT & UCBUSY);                // Wait for TX complete
  status = UCB2RXBUF;                       // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB2TXBUF = strobe;                       // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB2STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB2IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB2TXBUF = TI_CCxxx0_SRES;               // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB2STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

// End of support for 5xx USCI_B2
#endif

// End of support for USCI_B2

//******************************************************************************
// If USCIB3 is used
//   |-- If 5xx
//         |-- Use 5xx Init
//******************************************************************************
#elif TI_CC_RF_SER_INTF == TI_CC_SER_INTF_USCIB3

//******************************************************************************
// Support for 5xx USCI_B3
//******************************************************************************
#ifdef TI_5xx
void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  UCB3CTL1 |= UCSWRST;                      // **Disable USCI state machine**
  UCB3CTL0 |= UCMST+UCCKPH+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
  UCB3CTL1 |= UCSSEL_2;                     // SMCLK
  UCB3BR0 = 0x02;                           // UCLK/2
  UCB3BR1 = 0;
  TI_CC_SPI_USCIB3_PxSEL |= TI_CC_SPI_USCIB3_SOMI
                         | TI_CC_SPI_USCIB3_UCLK
                         | TI_CC_SPI_USCIB3_SIMO;
                                            // SPI option select
  TI_CC_SPI_USCIB3_PxDIR |= TI_CC_SPI_USCIB3_UCLK | TI_CC_SPI_USCIB3_SIMO;
                                            // SPI TXD out direction
  UCB3CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB3TXBUF = addr;                         // Send address
  while (!(UCB3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB3TXBUF = value;                        // Send data
  while (UCB3STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB3TXBUF = addr | TI_CCxxx0_WRITE_BURST; // Send address
  for (i = 0; i < count; i++)
  {
    while (!(UCB3IFG&UCTXIFG));             // Wait for TXBUF ready
    UCB3TXBUF = buffer[i];                  // Send data
  }
  while (UCB3STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB3TXBUF = (addr | TI_CCxxx0_READ_SINGLE);// Send address
  while (!(UCB3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB3TXBUF = 0;                            // Dummy write so we can read data
  while (UCB3STAT & UCBUSY);                // Wait for TX complete
  x = UCB3RXBUF;                            // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB3TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (UCB3STAT & UCBUSY);                // Wait for TX complete
  UCB3TXBUF = 0;                            // Dummy write to read 1st data byte
  // Addr byte is now being TX'ed, with dummy byte to follow immediately after
  UCB3IFG &= ~UCRXIFG;                      // Clear flag
  while (!(UCB3IFG&UCRXIFG));               // Wait for end of addr byte TX
  // First data byte now in RXBUF
  for (i = 0; i < (count-1); i++)
  {
    UCB3TXBUF = 0;                          //Initiate next data RX, meanwhile..
    buffer[i] = UCB3RXBUF;                  // Store data from last data RX
    while (!(UCB3IFG&UCRXIFG));             // Wait for RX to finish
  }
  buffer[count-1] = UCB3RXBUF;              // Store last RX byte in buffer
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t status;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB3TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (!(UCB3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB3TXBUF = 0;                            // Dummy write so we can read data
  while (UCB3STAT & UCBUSY);                // Wait for TX complete
  status = UCB3RXBUF;                       // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return status;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB3TXBUF = strobe;                       // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB3STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (!(UCB3IFG&UCTXIFG));               // Wait for TXBUF ready
  UCB3TXBUF = TI_CCxxx0_SRES;               // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB3STAT & UCBUSY);                // Wait for TX complete
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

// End of support for 5xx USCI_B3
#endif
// End of support for USCI_B3

//******************************************************************************
// If USI is used
//******************************************************************************
#elif TI_CC_RF_SER_INTF == TI_CC_SER_INTF_USI


void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  USICTL0 |= USIPE7 +  USIPE6 + USIPE5 + USIMST + USIOE;// Port, SPI master
  USICKCTL = USIDIV2 + USISSEL_2 + USICKPL; // SCLK = SMCLK/16
  USICTL0 &= ~USISWRST;                     // USI released for operation

  USISRL = 0x00;                            // Ensure SDO low instead of high,
  USICNT = 1;                               // to avoid conflict with CCxxxx
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (TI_CC_SPI_USI_PxIN&TI_CC_SPI_USI_SOMI);// Wait for CCxxxx ready
  USISRL = addr;                            // Load address
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
  USISRL = value;                           // Load value
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (TI_CC_SPI_USI_PxIN&TI_CC_SPI_USI_SOMI);// Wait for CCxxxx ready
  USISRL = addr | TI_CCxxx0_WRITE_BURST;    // Load address
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
  for (i = 0; i < count; i++)
  {
    USISRL = buffer[i];                     // Load data
    USICNT = 8;                             // Send it
    while (!(USICTL1&USIIFG));              // Wait for TX to finish
  }
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (TI_CC_SPI_USI_PxIN&TI_CC_SPI_USI_SOMI);// Wait for CCxxxx ready
  USISRL = addr | TI_CCxxx0_READ_SINGLE;    // Load address
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
  USICNT = 8;                               // Dummy write so we can read data
  while (!(USICTL1&USIIFG));                // Wait for RX to finish
  x = USISRL;                               // Store data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (TI_CC_SPI_USI_PxIN&TI_CC_SPI_USI_SOMI);// Wait for CCxxxx ready
  USISRL = addr | TI_CCxxx0_READ_BURST;     // Load address
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
  for (i = 0; i < count; i++)
  {
    USICNT = 8;                             // Dummy write so we can read data
    while (!(USICTL1&USIIFG));              // Wait for RX to finish
    buffer[i] = USISRL;                     // Store data
  }
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (TI_CC_SPI_USI_PxIN&TI_CC_SPI_USI_SOMI);// Wait for CCxxxx ready
  USISRL = addr | TI_CCxxx0_READ_BURST;     // Load address
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
  USICNT = 8;                               // Dummy write so we can read data
  while (!(USICTL1&USIIFG));                // Wait for RX to finish
  x = USISRL;                               // Store data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (TI_CC_SPI_USI_PxIN&TI_CC_SPI_USI_SOMI);// Wait for CCxxxx ready
  USISRL = strobe;                          // Load strobe
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  // Sec. 27.1 of CC1100 datasheet
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  while (TI_CC_SPI_USI_PxIN&TI_CC_SPI_USI_SOMI);
  USISRL = TI_CCxxx0_SRES;
  USICNT = 8;
  while (!(USICTL1&USIIFG));
  while (TI_CC_SPI_USI_PxIN&TI_CC_SPI_USI_SOMI);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
}

// End of USI

//******************************************************************************
// If Bit Bang is used
//******************************************************************************
#elif TI_CC_RF_SER_INTF == TI_CC_SER_INTF_BITBANG

void TI_CC_SPI_bitbang_out(uint8_t);
uint8_t TI_CC_SPI_bitbang_in();

void TI_CC_SPISetup(void)
{
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_CSn_PxDIR |= TI_CC_CSn_PIN;         // /CS disable

  // Config bitbang pins
  TI_CC_SPI_BITBANG_PxOUT |= TI_CC_SPI_BITBANG_SIMO;
  TI_CC_SPI_BITBANG_PxOUT &= ~TI_CC_SPI_BITBANG_UCLK;
  TI_CC_SPI_BITBANG_PxDIR |= TI_CC_SPI_BITBANG_SIMO | TI_CC_SPI_BITBANG_UCLK;
}

// Output eight-bit value using selected bit-bang pins
void TI_CC_SPI_bitbang_out(uint8_t value)
{
  uint8_t x;

  for(x=8;x>0;x--)
  {
    if(value & 0x80)                                     // If bit is high...
      TI_CC_SPI_BITBANG_PxOUT |= TI_CC_SPI_BITBANG_SIMO; // Set SIMO high...
    else
      TI_CC_SPI_BITBANG_PxOUT &= ~TI_CC_SPI_BITBANG_SIMO;// Set SIMO low...
    value = value << 1;                                  // Rotate bits

    TI_CC_SPI_BITBANG_PxOUT &= ~TI_CC_SPI_BITBANG_UCLK;  // Set clock low
    TI_CC_SPI_BITBANG_PxOUT |= TI_CC_SPI_BITBANG_UCLK;   // Set clock high
  }
  TI_CC_SPI_BITBANG_PxOUT &= ~TI_CC_SPI_BITBANG_UCLK;  // Set clock low
}

// Input eight-bit value using selected bit-bang pins
uint8_t TI_CC_SPI_bitbang_in()
{
  uint8_t x=0,shift=0;
  int y;

  // Determine how many bit positions SOMI is from least-significant bit
  x = TI_CC_SPI_BITBANG_SOMI;
  while(x>1)
  {
    shift++;
    x = x >> 1;
  };

  x = 0;
  for(y=8;y>0;y--)
  {
    TI_CC_SPI_BITBANG_PxOUT &= ~TI_CC_SPI_BITBANG_UCLK;// Set clock low
    TI_CC_SPI_BITBANG_PxOUT |= TI_CC_SPI_BITBANG_UCLK;// Set clock high

    x = x << 1;                             // Make room for next bit
    x = x | ((TI_CC_SPI_BITBANG_PxIN & TI_CC_SPI_BITBANG_SOMI) >> shift);
  }                                         // Store next bit
  TI_CC_SPI_BITBANG_PxOUT &= ~TI_CC_SPI_BITBANG_UCLK; // Set clock low
  return(x);
}

void TI_CC_SPIWriteReg(uint8_t addr, uint8_t value)
{
    TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;      // /CS enable
    while (TI_CC_SPI_BITBANG_PxIN&TI_CC_SPI_BITBANG_SOMI); // Wait CCxxxx ready
    TI_CC_SPI_bitbang_out(addr);            // Send address
    TI_CC_SPI_bitbang_out(value);           // Send data
    TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;       // /CS disable
}

void TI_CC_SPIWriteBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
    uint8_t i;

    TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;      // /CS enable
    while (TI_CC_SPI_BITBANG_PxIN&TI_CC_SPI_BITBANG_SOMI); // Wait CCxxxx ready
    TI_CC_SPI_bitbang_out(addr | TI_CCxxx0_WRITE_BURST);   // Send address
    for (i = 0; i < count; i++)
      TI_CC_SPI_bitbang_out(buffer[i]);     // Send data
    TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;       // /CS disable
}

uint8_t TI_CC_SPIReadReg(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  TI_CC_SPI_bitbang_out(addr | TI_CCxxx0_READ_SINGLE);//Send address
  x = TI_CC_SPI_bitbang_in();               // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIReadBurstReg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (TI_CC_SPI_BITBANG_PxIN&TI_CC_SPI_BITBANG_SOMI); // Wait CCxxxx ready
  TI_CC_SPI_bitbang_out(addr | TI_CCxxx0_READ_BURST);    // Send address
  for (i = 0; i < count; i++)
    buffer[i] = TI_CC_SPI_bitbang_in();     // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

uint8_t TI_CC_SPIReadStatus(uint8_t addr)
{
  uint8_t x;

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (TI_CC_SPI_BITBANG_PxIN & TI_CC_SPI_BITBANG_SOMI); // Wait CCxxxx ready
  TI_CC_SPI_bitbang_out(addr | TI_CCxxx0_READ_BURST);      // Send address
  x = TI_CC_SPI_bitbang_in();               // Read data
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable

  return x;
}

void TI_CC_SPIStrobe(uint8_t strobe)
{
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;        // /CS enable
  while (TI_CC_SPI_BITBANG_PxIN&TI_CC_SPI_BITBANG_SOMI);// Wait for CCxxxx ready
  TI_CC_SPI_bitbang_out(strobe);            // Send strobe
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;         // /CS disable
}

void TI_CC_PowerupResetCCxxxx(void)
{
  // Sec. 27.1 of CC1100 datasheet
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  TI_CC_Wait(30);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
  TI_CC_Wait(45);

  TI_CC_CSn_PxOUT &= ~TI_CC_CSn_PIN;
  while (TI_CC_SPI_BITBANG_PxIN&TI_CC_SPI_BITBANG_SOMI);
  TI_CC_SPI_bitbang_out(TI_CCxxx0_SRES);
  while (TI_CC_SPI_BITBANG_PxIN&TI_CC_SPI_BITBANG_SOMI);
  TI_CC_CSn_PxOUT |= TI_CC_CSn_PIN;
}
#endif
