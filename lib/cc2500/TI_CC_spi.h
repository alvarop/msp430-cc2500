//------------------------------------------------------------------------------
//  Description:  Header file for TI_CC_spi.c
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
// Comments: Fixed function bugs
//           Added support for 5xx
//
// Version:  1.00
// Comments: Initial Release Version
//------------------------------------------------------------------------------

void TI_CC_SPISetup(void);
void TI_CC_PowerupResetCCxxxx(void);
void TI_CC_SPIWriteReg(uint8_t, uint8_t);
void TI_CC_SPIWriteBurstReg(uint8_t, uint8_t*, uint8_t);
uint8_t TI_CC_SPIReadReg(uint8_t);
void TI_CC_SPIReadBurstReg(uint8_t, uint8_t *, uint8_t);
uint8_t TI_CC_SPIReadStatus(uint8_t);
void TI_CC_SPIStrobe(uint8_t);
void TI_CC_Wait(uint16_t);




