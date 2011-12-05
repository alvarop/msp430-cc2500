//------------------------------------------------------------------------------
//  Description:  This file contains functions that configure the CC1100/2500
//  device.
//
//  Demo Application for MSP430/CC1100-2500 Interface Code Library v1.0
//
//  K. Quiring
//  Texas Instruments, Inc.
//  July 2006
//  IAR Embedded Workbench v3.41
//------------------------------------------------------------------------------


#include "TI_CC_include.h"
#include "TI_CC_CC1100-CC2500.h"

#define TI_CC_RF_FREQ  2400                 // 315, 433, 868, 915, 2400



//------------------------------------------------------------------------------
//  void writeRFSettings(void)
//
//  DESCRIPTION:
//  Used to configure the CCxxxx registers.  There are five instances of this
//  function, one for each available carrier frequency.  The instance compiled
//  is chosen according to the system variable TI_CC_RF_FREQ, assigned within
//  the header file "TI_CC_hardware_board.h".
//
//  ARGUMENTS:
//      none
//------------------------------------------------------------------------------


// Product = CC2500
// Crystal accuracy = 40 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 540.000000 kHz
// Deviation = 0.000000
// Return state:  Return to RX state upon leaving either TX or RX
// Datarate = 250.000000 kbps
// Modulation = (7) MSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 2433.000000 MHz
// Channel spacing = 199.950000 kHz
// Channel number = 0
// Optimization = Sensitivity
// Sync mode = (3) 30/32 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction = (0) FEC disabled
// Length configuration = (1) Variable length packets, packet length configured by the first received byte after sync word.
// Packetlength = 255
// Preamble count = (2)  4 bytes
// Append status = 1
// Address check = Address check and 0 (0x00) broadcast
// CRC autoflush = true
// Device address = 1
// GDO0 signal selection = ( 0x06 ) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = ( 0x0E ) Carrier sense. High if RSSI level is above threshold.
void writeRFSettings(void)
{

  // Write register settings
  TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG2,   0x0E);  // GDO2 output pin config.
  TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG0,   0x06);  // GDO0 output pin config.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTLEN,   0x3D);  // Packet length.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL1, 0x0E);  // Packet automation control.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL0, 0x05);  // Packet automation control.
  TI_CC_SPIWriteReg(TI_CCxxx0_ADDR,     0x01);  // Device address.
  TI_CC_SPIWriteReg(TI_CCxxx0_CHANNR,   0x00); // Channel number.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL1,  0x07); // Freq synthesizer control.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL0,  0x00); // Freq synthesizer control.
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ2,    0x5D); // Freq control word, high byte
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ1,    0x93); // Freq control word, mid byte.
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ0,    0xB1); // Freq control word, low byte.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG4,  0x2D); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG3,  0x3B); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG2,  0x73); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG1,  0x22); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG0,  0xF8); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_DEVIATN,  0x00); // Modem dev (when FSK mod en)
  TI_CC_SPIWriteReg(TI_CCxxx0_MCSM1 ,   0x2F); //MainRadio Cntrl State Machine
  TI_CC_SPIWriteReg(TI_CCxxx0_MCSM0 ,   0x18); //MainRadio Cntrl State Machine
  TI_CC_SPIWriteReg(TI_CCxxx0_FOCCFG,   0x1D); // Freq Offset Compens. Config
  TI_CC_SPIWriteReg(TI_CCxxx0_BSCFG,    0x1C); //  Bit synchronization config.
  TI_CC_SPIWriteReg(TI_CCxxx0_AGCCTRL2, 0xC7); // AGC control.
  TI_CC_SPIWriteReg(TI_CCxxx0_AGCCTRL1, 0x00); // AGC control.
  TI_CC_SPIWriteReg(TI_CCxxx0_AGCCTRL0, 0xB2); // AGC control.
  TI_CC_SPIWriteReg(TI_CCxxx0_FREND1,   0xB6); // Front end RX configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_FREND0,   0x10); // Front end RX configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL3,   0xEA); // Frequency synthesizer cal.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL2,   0x0A); // Frequency synthesizer cal.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL1,   0x00); // Frequency synthesizer cal.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCAL0,   0x11); // Frequency synthesizer cal.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSTEST,   0x59); // Frequency synthesizer cal.
  TI_CC_SPIWriteReg(TI_CCxxx0_TEST2,    0x88); // Various test settings.
  TI_CC_SPIWriteReg(TI_CCxxx0_TEST1,    0x31); // Various test settings.
  TI_CC_SPIWriteReg(TI_CCxxx0_TEST0,    0x0B);  // Various test settings.
}
