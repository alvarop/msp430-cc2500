//******************************************************************************
//  Description:  This file contains definitions specific to the hardware board.
//  Specifically, the definitions include hardware connections with the
//  CCxxxx connector port, LEDs, and switches.
//
//  MSP430/CC1100-2500 Interface Code Library v1.1
//
//  W. Goh
//  Texas Instruments, Inc.
//  December 2009
//  Built with IAR Embedded Workbench Version: 4.20
//******************************************************************************
// Change Log:
//******************************************************************************
// Version:  1.1
// Comments: Added support for eZ430-RF2500 board
// Version:  1.00
// Comments: Initial Release Version
//******************************************************************************

#define TI_CC_LED_PxOUT         P1OUT
#define TI_CC_LED_PxDIR         P1DIR
#define TI_CC_LED1              BIT0
#define TI_CC_LED2              BIT1

#define TI_CC_SW_PxIN           P1IN
#define TI_CC_SW_PxIE           P1IE
#define TI_CC_SW_PxIES          P1IES
#define TI_CC_SW_PxIFG          P1IFG
#define TI_CC_SW_PxREN          P1REN
#define TI_CC_SW_PxOUT          P1OUT
#define TI_CC_SW1               BIT2

#define TI_CC_GDO0_PxOUT        P2OUT
#define TI_CC_GDO0_PxIN         P2IN
#define TI_CC_GDO0_PxDIR        P2DIR
#define TI_CC_GDO0_PxIE         P2IE
#define TI_CC_GDO0_PxIES        P2IES
#define TI_CC_GDO0_PxIFG        P2IFG
#define TI_CC_GDO0_PIN          BIT6

#define TI_CC_GDO1_PxOUT        P3OUT
#define TI_CC_GDO1_PxIN         P3IN
#define TI_CC_GDO1_PxDIR        P3DIR
#define TI_CC_GDO1_PIN          0x04

#define TI_CC_GDO2_PxOUT        P2OUT
#define TI_CC_GDO2_PxIN         P2IN
#define TI_CC_GDO2_PxDIR        P2DIR
#define TI_CC_GDO2_PIN          BIT7

#define TI_CC_CSn_PxOUT         P3OUT
#define TI_CC_CSn_PxDIR         P3DIR
#define TI_CC_CSn_PIN           BIT0


//******************************************************************************
// Select which port will be used for interface to CCxxxx
//******************************************************************************
#define TI_CC_RF_SER_INTF       TI_CC_SER_INTF_USCIB0  // Interface to CCxxxx

