//******************************************************************************
//  Description:  Master include file
//
//  Demo Application for MSP430/CC1100-2500 Interface Code Library v1.1
//
// W. Goh
// Texas Instruments, Inc
// December 2009
// Built with IAR Embedded Workbench Version: 4.20
//******************************************************************************
// Change Log:
//******************************************************************************
// Version:  1.1
// Comments: Added support for various MSP430 development tools
// Version:  1.00
// Comments: Initial Release Version
//******************************************************************************
#include <stdint.h>

#include "TI_CC_CC1100-CC2500.h"
#include "TI_CC_spi.h"
#include "CC1100-CC2500.h"

#include "TI_CC_msp430.h"

// Uncomment/Comment out depending on which experimenters board is being used
#include "TI_CC_hardware_board.h"
//#include "TI_CC_hardware_board_eZ430.h"
//#include "TI_CC_hardware_board_EXP4618.h"
//#include "TI_CC_hardware_board_EXP5438.h"

