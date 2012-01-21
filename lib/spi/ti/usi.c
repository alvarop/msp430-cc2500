/** @file usi.c
*
* @brief SPI and CCxxxx radio communication functions
*         This file uses the USI SPI peripheral
*
* @author Alvaro Prieto
*/
#include "spi.h"
#include "device.h"
#if !defined(SPI_INTERFACE_USI)
#error This serial library was written for device with USI
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

  USICTL0 |= USIPE7 +  USIPE6 + USIPE5 + USIMST + USIOE;// Port, SPI master
  USICKCTL = USIDIV2 + USISSEL_2 + USICKPL; // SCLK = SMCLK/16
  USICTL0 &= ~USISWRST;                     // USI released for operation

  USISRL = 0x00;                            // Ensure SDO low instead of high,
  USICNT = 1;                               // to avoid conflict with CCxxxx
}

/*******************************************************************************
 * @fn void cc_write_reg(uint8_t addr, uint8_t value)
 * @brief Write single register value to CCxxxx
 * ****************************************************************************/
void cc_write_reg(uint8_t addr, uint8_t value)
{
  CSn_PxOUT &= ~CSn_PIN;        // /CS enable
  while (SPI_USI_PxIN&SPI_USI_SOMI);// Wait for CCxxxx ready
  USISRL = addr;                            // Load address
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
  USISRL = value;                           // Load value
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
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
  while (SPI_USI_PxIN&SPI_USI_SOMI);// Wait for CCxxxx ready
  USISRL = addr | TI_CCxxx0_WRITE_BURST;    // Load address
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
  for (i = 0; i < count; i++)
  {
    USISRL = buffer[i];                     // Load data
    USICNT = 8;                             // Send it
    while (!(USICTL1&USIIFG));              // Wait for TX to finish
  }
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
  while (SPI_USI_PxIN&SPI_USI_SOMI);// Wait for CCxxxx ready
  USISRL = addr | TI_CCxxx0_READ_SINGLE;    // Load address
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
  USICNT = 8;                               // Dummy write so we can read data
  while (!(USICTL1&USIIFG));                // Wait for RX to finish
  x = USISRL;                               // Store data
  CSn_PxOUT |= CSn_PIN;         // /CS disable

  return x;
}

/*******************************************************************************
 * @fn cc_read_burst_reg(uint8_t addr, uint8_t *buffer, uint8_t count)
 * @brief read multiple registers from CCxxxx
 * ****************************************************************************/
void cc_read_burst_reg(uint8_t addr, uint8_t *buffer, uint8_t count)
{
  uint16_t i;

  CSn_PxOUT &= ~CSn_PIN;        // /CS enable
  while (SPI_USI_PxIN&SPI_USI_SOMI);// Wait for CCxxxx ready
  USISRL = addr | TI_CCxxx0_READ_BURST;     // Load address
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
  for (i = 0; i < count; i++)
  {
    USICNT = 8;                             // Dummy write so we can read data
    while (!(USICTL1&USIIFG));              // Wait for RX to finish
    buffer[i] = USISRL;                     // Store data
  }
  CSn_PxOUT |= CSn_PIN;         // /CS disable
}

/*******************************************************************************
 * @fn uint8_t cc_read_status(uint8_t addr)
 * @brief send status command and read returned status byte
 * ****************************************************************************/
uint8_t cc_read_status(uint8_t addr)
{
  uint8_t x;

  CSn_PxOUT &= ~CSn_PIN;        // /CS enable
  while (SPI_USI_PxIN&SPI_USI_SOMI);// Wait for CCxxxx ready
  USISRL = addr | TI_CCxxx0_READ_BURST;     // Load address
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
  USICNT = 8;                               // Dummy write so we can read data
  while (!(USICTL1&USIIFG));                // Wait for RX to finish
  x = USISRL;                               // Store data
  CSn_PxOUT |= CSn_PIN;         // /CS disable

  return x;
}

/*******************************************************************************
 * @fn void cc_strobe (uint8_t strobe)
 * @brief send strobe command
 * ****************************************************************************/
void cc_strobe(uint8_t strobe)
{
  CSn_PxOUT &= ~CSn_PIN;        // /CS enable
  while (SPI_USI_PxIN&SPI_USI_SOMI);// Wait for CCxxxx ready
  USISRL = strobe;                          // Load strobe
  USICNT = 8;                               // Send it
  while (!(USICTL1&USIIFG));                // Wait for TX to finish
  CSn_PxOUT |= CSn_PIN;         // /CS disable
}

/*******************************************************************************
 * @fn void cc_powerup_reset()
 * @brief reset radio
 * ****************************************************************************/
void cc_powerup_reset(void)
{
  // Sec. 27.1 of CC1100 datasheet
  CSn_PxOUT |= CSn_PIN;
  wait_cycles(30);
  CSn_PxOUT &= ~CSn_PIN;
  wait_cycles(30);
  CSn_PxOUT |= CSn_PIN;
  wait_cycles(45);

  CSn_PxOUT &= ~CSn_PIN;
  while (SPI_USI_PxIN&SPI_USI_SOMI);
  USISRL = TI_CCxxx0_SRES;
  USICNT = 8;
  while (!(USICTL1&USIIFG));
  while (SPI_USI_PxIN&SPI_USI_SOMI);
  CSn_PxOUT |= CSn_PIN;
}
