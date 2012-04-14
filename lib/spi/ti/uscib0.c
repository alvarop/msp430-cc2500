/** @file uscib0.c
*
* @brief SPI and CCxxxx radio communication functions
*         This file uses the USCIB0 SPI peripheral
*
* @author Alvaro Prieto
*/
#include "spi.h"
#include "device.h"
#if !defined(SPI_INTERFACE_USCIB0)
#error This SPI library was written for device with USCI B0
#endif

void wait_cycles(uint16_t cycles)
{
  while(cycles>15)                          // 15 cycles consumed by overhead
    cycles = cycles - 6;                    // 6 cycles consumed each iteration
}

/*******************************************************************************
 * @fn void spi_setup(void)
 * @brief Setup SPI with the appropriate settings for CCxxxx communication
 * ****************************************************************************/
void spi_setup(void)
{
  CSn_PxOUT |= CSn_PIN;
  CSn_PxDIR |= CSn_PIN;         // /CS disable

  UCB0CTL1 |= UCSWRST;                      // **Disable USCI state machine**
  UCB0CTL0 |= UCMST+UCCKPL+UCMSB+UCSYNC;    // 3-pin, 8-bit SPI master
  UCB0CTL1 |= UCSSEL_2;                     // SMCLK
  UCB0BR0 = 0x10;                           // UCLK/2
  UCB0BR1 = 0;
  SPI_USCIB0_PxSEL  |= SPI_USCIB0_SIMO
                          | SPI_USCIB0_SOMI
                          | SPI_USCIB0_UCLK;

  SPI_USCIB0_PxSEL2 |= SPI_USCIB0_SIMO
                          | SPI_USCIB0_SOMI
                          | SPI_USCIB0_UCLK;

                                            // SPI option select
  //SPI_USCIB0_PxDIR |= SPI_USCIB0_SIMO | SPI_USCIB0_UCLK;
                                            // SPI TXD out direction
  UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}

/*******************************************************************************
 * @fn void cc_write_reg(uint8_t addr, uint8_t value)
 * @brief Write single register value to CCxxxx
 * ****************************************************************************/
void cc_write_reg(uint8_t addr, uint8_t value)
{
  CSn_PxOUT &= ~CSn_PIN;        // /CS enable
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = addr;                         // Send address
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = value;                        // Send data
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  CSn_PxOUT |= CSn_PIN;         // /CS disable
}

/*******************************************************************************
 * @fn cc_write_burst_reg(uint8_t addr, uint8_t *buffer, uint8_t count)
 * @brief Write multiple values to CCxxxx
 * ****************************************************************************/
void cc_write_burst_reg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  CSn_PxOUT &= ~CSn_PIN;        // /CS enable
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = addr | TI_CCxxx0_WRITE_BURST; // Send address
  for (i = 0; i < count; i++)
  {
    while (!(IFG2&UCB0TXIFG));              // Wait for TXBUF ready
    UCB0TXBUF = buffer[i];                  // Send data
  }
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  CSn_PxOUT |= CSn_PIN;         // /CS disable
}

/*******************************************************************************
 * @fn uint8_t cc_read_reg(uint8_t addr)
 * @brief read single register from CCxxxx
 * ****************************************************************************/
uint8_t cc_read_reg(uint8_t addr)
{
  uint8_t x;

  CSn_PxOUT &= ~CSn_PIN;        // /CS enable
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = (addr | TI_CCxxx0_READ_SINGLE);// Send address
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = 0;                            // Dummy write so we can read data
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  CSn_PxOUT |= CSn_PIN;         // /CS disable
  x = UCB0RXBUF;                            // Read data

  return x;
}

/*******************************************************************************
 * @fn cc_read_burst_reg(uint8_t addr, uint8_t *buffer, uint8_t count)
 * @brief read multiple registers from CCxxxx
 * ****************************************************************************/
void cc_read_burst_reg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint8_t i;

  CSn_PxOUT &= ~CSn_PIN;        // /CS enable
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
  CSn_PxOUT |= CSn_PIN;         // /CS disable
}

/*******************************************************************************
 * @fn uint8_t cc_read_status(uint8_t addr)
 * @brief send status command and read returned status byte
 * ****************************************************************************/
uint8_t cc_read_status(uint8_t addr)
{
  uint8_t status;

  CSn_PxOUT &= ~CSn_PIN;        // /CS enable
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = (addr | TI_CCxxx0_READ_BURST);// Send address
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = 0;                            // Dummy write so we can read data
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  status = UCB0RXBUF;                       // Read data
  CSn_PxOUT |= CSn_PIN;         // /CS disable

  return status;
}

/*******************************************************************************
 * @fn void cc_strobe (uint8_t strobe)
 * @brief send strobe command
 * ****************************************************************************/
void cc_strobe(uint8_t strobe)
{
  CSn_PxOUT &= ~CSn_PIN;        // /CS enable
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = strobe;                       // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  CSn_PxOUT |= CSn_PIN;         // /CS disable
}
/*******************************************************************************
 * @fn void cc_powerup_reset()
 * @brief reset radio
 * ****************************************************************************/
void cc_powerup_reset(void)
{
  CSn_PxOUT |= CSn_PIN;
  wait_cycles(30);
  CSn_PxOUT &= ~CSn_PIN;
  wait_cycles(30);
  CSn_PxOUT |= CSn_PIN;
  wait_cycles(45);

  CSn_PxOUT &= ~CSn_PIN;        // /CS enable
  while (!(IFG2&UCB0TXIFG));                // Wait for TXBUF ready
  UCB0TXBUF = TI_CCxxx0_SRES;               // Send strobe
  // Strobe addr is now being TX'ed
  while (UCB0STAT & UCBUSY);                // Wait for TX to complete
  while(SPI_USCIB0_PxIN & SPI_USCIB0_SOMI); // Wait until the device has reset
  CSn_PxOUT |= CSn_PIN;         // /CS disable
}
