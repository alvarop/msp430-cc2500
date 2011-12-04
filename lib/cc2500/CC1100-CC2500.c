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


#if TI_CC_RF_FREQ == 315                          // 315 MHz
// Product = CC1100
// Crystal accuracy = 40 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 540.000000 kHz
// Deviation = 0.000000
// Return state:  Return to RX state upon leaving either TX or RX
// Datarate = 250.000000 kbps
// Modulation = (7) MSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 315.000000 MHz
// Channel spacing = 199.951172 kHz
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
// Address check = (1)  address check
// FIFO autoflush = 0
// Device address = 1
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = (11) Serial Clock
void writeRFSettings(void)
{
    // Write register settings
  TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG2,   0x0B); // GDO2 output pin config.
  TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG0,   0x06); // GDO0 output pin config.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTLEN,   0xFF); // Packet length.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL1, 0x05); // Packet automation control.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL0, 0x05); // Packet automation control.
  TI_CC_SPIWriteReg(TI_CCxxx0_ADDR,     0x01); // Device address.
  TI_CC_SPIWriteReg(TI_CCxxx0_CHANNR,   0x00); // Channel number.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL1,  0x0B); // Freq synthesizer control.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL0,  0x00); // Freq synthesizer control.
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ2,    0x0C); // Freq control word, high byte
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ1,    0x1D); // Freq control word, mid byte.
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ0,    0x89); // Freq control word, low byte.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG4,  0x2D); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG3,  0x3B); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG2,  0x73); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG1,  0x22); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG0,  0xF8); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_DEVIATN,  0x00); // Modem dev (when FSK mod en)
  TI_CC_SPIWriteReg(TI_CCxxx0_MCSM1 ,   0x3F); //MainRadio Cntrl State Machine
  TI_CC_SPIWriteReg(TI_CCxxx0_MCSM0 ,   0x18); //MainRadio Cntrl State Machine
  TI_CC_SPIWriteReg(TI_CCxxx0_FOCCFG,   0x1D); // Freq Offset Compense. Config
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
  TI_CC_SPIWriteReg(TI_CCxxx0_TEST0,    0x0B); // Various test settings.
}

// PATABLE (0 dBm output power)
extern char paTable[] = {0x51};
extern char paTableLen = 1;

#endif


#if TI_CC_RF_FREQ == 433                          // 433 MHz
// Product = CC1100
// Crystal accuracy = 40 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 540.000000 kHz
// Deviation = 0.000000
// Return state:  Return to RX state upon leaving either TX or RX
// Datarate = 250.000000 kbps
// Modulation = (7) MSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 433.000000 MHz
// Channel spacing = 199.951172 kHz
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
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = (11) Serial Clock
void writeRFSettings(void)
{
    // Write register settings
  TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG2,   0x0B); // GDO2 output pin config.
  TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG0,   0x06); // GDO0 output pin config.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTLEN,   0xFF); // Packet length.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL1, 0x05); // Packet automation control.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL0, 0x05); // Packet automation control.
  TI_CC_SPIWriteReg(TI_CCxxx0_ADDR,     0x01); // Device address.
  TI_CC_SPIWriteReg(TI_CCxxx0_CHANNR,   0x00); // Channel number.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL1,  0x0B); // Freq synthesizer control.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL0,  0x00); // Freq synthesizer control.
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ2,    0x10); // Freq control word, high byte
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ1,    0xA7); // Freq control word, mid byte.
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ0,    0x62); // Freq control word, low byte.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG4,  0x2D); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG3,  0x3B); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG2,  0x73); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG1,  0x22); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG0,  0xF8); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_DEVIATN,  0x00); // Modem dev (when FSK mod en)
  TI_CC_SPIWriteReg(TI_CCxxx0_MCSM1 ,   0x3F); //MainRadio Cntrl State Machine
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
  TI_CC_SPIWriteReg(TI_CCxxx0_TEST0,    0x0B); // Various test settings.
}

// PATABLE (0 dBm output power)
extern char paTable[] = {0x51};
extern char paTableLen = 1;

#endif


#if TI_CC_RF_FREQ == 868                          // 868 MHz
// Product = CC1100
// Crystal accuracy = 40 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 540.000000 kHz
// Deviation = 0.000000
// Return state:  Return to RX state upon leaving either TX or RX
// Datarate = 250.000000 kbps
// Modulation = (7) MSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 868.000000 MHz
// Channel spacing = 199.951172 kHz
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
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = (11) Serial Clock
void writeRFSettings(void)
{
    // Write register settings
  TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG2,   0x0B); // GDO2 output pin config.
  TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG0,   0x06); // GDO0 output pin config.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTLEN,   0xFF); // Packet length.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL1, 0x05); // Packet automation control.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL0, 0x05); // Packet automation control.
  TI_CC_SPIWriteReg(TI_CCxxx0_ADDR,     0x01); // Device address.
  TI_CC_SPIWriteReg(TI_CCxxx0_CHANNR,   0x00); // Channel number.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL1,  0x0B); // Freq synthesizer control.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL0,  0x00); // Freq synthesizer control.
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ2,    0x21); // Freq control word, high byte
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ1,    0x62); // Freq control word, mid byte.
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ0,    0x76); // Freq control word, low byte.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG4,  0x2D); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG3,  0x3B); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG2,  0x73); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG1,  0x22); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG0,  0xF8); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_DEVIATN,  0x00); // Modem dev (when FSK mod en)
  TI_CC_SPIWriteReg(TI_CCxxx0_MCSM1 ,   0x3F); //MainRadio Cntrl State Machine
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
  TI_CC_SPIWriteReg(TI_CCxxx0_TEST0,    0x0B); // Various test settings.
}

// PATABLE (0 dBm output power)
extern char paTable[] = {0x60};
extern char paTableLen = 1;

#endif


#if TI_CC_RF_FREQ == 915                          // 915 MHz
// Product = CC1100
// Crystal accuracy = 40 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 540.000000 kHz
// Deviation = 0.000000
// Return state:  Return to RX state upon leaving either TX or RX
// Datarate = 250.000000 kbps
// Modulation = (7) MSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 915.000000 MHz
// Channel spacing = 199.951172 kHz
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
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = (11) Serial Clock
void writeRFSettings(void)
{
    // Write register settings
  TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG2,   0x0B); // GDO2 output pin config.
  TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG0,   0x06); // GDO0 output pin config.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTLEN,   0xFF); // Packet length.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL1, 0x05); // Packet automation control.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL0, 0x05); // Packet automation control.
  TI_CC_SPIWriteReg(TI_CCxxx0_ADDR,     0x01); // Device address.
  TI_CC_SPIWriteReg(TI_CCxxx0_CHANNR,   0x00); // Channel number.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL1,  0x0B); // Freq synthesizer control.
  TI_CC_SPIWriteReg(TI_CCxxx0_FSCTRL0,  0x00); // Freq synthesizer control.
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ2,    0x23); // Freq control word, high byte
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ1,    0x31); // Freq control word, mid byte.
  TI_CC_SPIWriteReg(TI_CCxxx0_FREQ0,    0x3B); // Freq control word, low byte.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG4,  0x2D); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG3,  0x3B); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG2,  0x73); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG1,  0x22); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_MDMCFG0,  0xF8); // Modem configuration.
  TI_CC_SPIWriteReg(TI_CCxxx0_DEVIATN,  0x00); // Modem dev (when FSK mod en)
  TI_CC_SPIWriteReg(TI_CCxxx0_MCSM1 ,   0x3F); //MainRadio Cntrl State Machine
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
  TI_CC_SPIWriteReg(TI_CCxxx0_TEST0,    0x0B); // Various test settings.
}

// PATABLE (0 dBm output power)
extern char paTable[] = {0x50};
extern char paTableLen = 1;

#endif



#if TI_CC_RF_FREQ == 2400                          // 2.4GHz
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
// Address check = (0) No address check
// FIFO autoflush = 0
// Device address = 0
// GDO0 signal selection = ( 6) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = (11) Serial Clock
void writeRFSettings(void)
{
    // Write register settings
  TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG2,   0x0B);  // GDO2 output pin config.
  TI_CC_SPIWriteReg(TI_CCxxx0_IOCFG0,   0x06);  // GDO0 output pin config.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTLEN,   0xFF);  // Packet length.
  TI_CC_SPIWriteReg(TI_CCxxx0_PKTCTRL1, 0x05);  // Packet automation control.
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
  TI_CC_SPIWriteReg(TI_CCxxx0_MCSM1 ,   0x3F); //MainRadio Cntrl State Machine
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

// PATABLE (0 dBm output power)
extern char paTable[] = {0xFB};
extern char paTableLen = 1;

#endif


//-----------------------------------------------------------------------------
//  void RFSendPacket(char *txBuffer, char size)
//
//  DESCRIPTION:
//  This function transmits a packet with length up to 63 bytes.  To use this
//  function, GD00 must be configured to be asserted when sync word is sent and
//  de-asserted at the end of the packet, which is accomplished by setting the
//  IOCFG0 register to 0x06, per the CCxxxx datasheet.  GDO0 goes high at
//  packet start and returns low when complete.  The function polls GDO0 to
//  ensure packet completion before returning.
//
//  ARGUMENTS:
//      char *txBuffer
//          Pointer to a buffer containing the data to be transmitted
//
//      char size
//          The size of the txBuffer
//-----------------------------------------------------------------------------
void RFSendPacket(char *txBuffer, char size)
{
  TI_CC_SPIWriteBurstReg(TI_CCxxx0_TXFIFO, txBuffer, size); // Write TX data
  TI_CC_SPIStrobe(TI_CCxxx0_STX);           // Change state to TX, initiating
                                            // data transfer

  while (!(TI_CC_GDO0_PxIN&TI_CC_GDO0_PIN));
                                            // Wait GDO0 to go hi -> sync TX'ed
  while (TI_CC_GDO0_PxIN&TI_CC_GDO0_PIN);
                                            // Wait GDO0 to clear -> end of pkt
  TI_CC_GDO0_PxIFG &= ~TI_CC_GDO0_PIN;      // After pkt TX, this flag is set.
                                            // Has to be cleared before existing
}



//-----------------------------------------------------------------------------
//  char RFReceivePacket(char *rxBuffer, char *length)
//
//  DESCRIPTION:
//  Receives a packet of variable length (first byte in the packet must be the
//  length byte).  The packet length should not exceed the RXFIFO size.  To use
//  this function, APPEND_STATUS in the PKTCTRL1 register must be enabled.  It
//  is assumed that the function is called after it is known that a packet has
//  been received; for example, in response to GDO0 going low when it is
//  configured to output packet reception status.
//
//  The RXBYTES register is first read to ensure there are bytes in the FIFO.
//  This is done because the GDO signal will go high even if the FIFO is flushed
//  due to address filtering, CRC filtering, or packet length filtering.
//
//  ARGUMENTS:
//      char *rxBuffer
//          Pointer to the buffer where the incoming data should be stored
//      char *length
//          Pointer to a variable containing the size of the buffer where the
//          incoming data should be stored. After this function returns, that
//          variable holds the packet length.
//
//  RETURN VALUE:
//      char
//          0x80:  CRC OK
//          0x00:  CRC NOT OK (or no pkt was put in the RXFIFO due to filtering)
//-----------------------------------------------------------------------------
char RFReceivePacket(char *rxBuffer, char *length)
{
  char status[2];
  char pktLen;

  if ((TI_CC_SPIReadStatus(TI_CCxxx0_RXBYTES) & TI_CCxxx0_NUM_RXBYTES))
  {
    pktLen = TI_CC_SPIReadReg(TI_CCxxx0_RXFIFO); // Read length byte

    if (pktLen <= *length)                  // If pktLen size <= rxBuffer
    {
      TI_CC_SPIReadBurstReg(TI_CCxxx0_RXFIFO, rxBuffer, pktLen); // Pull data
      *length = pktLen;                     // Return the actual size
      TI_CC_SPIReadBurstReg(TI_CCxxx0_RXFIFO, status, 2);
                                            // Read appended status bytes
      return (char)(status[TI_CCxxx0_LQI_RX]&TI_CCxxx0_CRC_OK);
    }                                       // Return CRC_OK bit
    else
    {
      *length = pktLen;                     // Return the large size
      TI_CC_SPIStrobe(TI_CCxxx0_SFRX);      // Flush RXFIFO
      return 0;                             // Error
    }
  }
  else
      return 0;                             // Error
}
