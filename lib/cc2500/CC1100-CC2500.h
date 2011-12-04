//------------------------------------------------------------------------------
//  Description:  CC1100-CC2500.c function declarations
//
//  Demo Application for MSP430/CC1100-2500 Interface Code Library v1.0
//
//  K. Quiring
//  Texas Instruments, Inc.
//  July 2006
//  IAR Embedded Workbench v3.41
//------------------------------------------------------------------------------


void writeRFSettings(void);
void RFSendPacket(uint8_t *, uint8_t);
uint8_t RFReceivePacket(uint8_t *, uint8_t *);
