//----------------------------------------------------------------------------
//  Description:  This file contains definitions specific to the hardware board.
//  Specifically, the definitions include hardware connections with the
//  CCxxxx connector port, LEDs, and switches.
//
//  MSP430/CC1100-2500 Interface Code Library v1.0
//
//  K. Quiring
//  Texas Instruments, Inc.
//  July 2006
//  IAR Embedded Workbench v3.41
//----------------------------------------------------------------------------

#define TI_CC_LED_PxOUT         P1OUT
#define TI_CC_LED_PxDIR         P1DIR
#define TI_CC_LED1              0x01
//#define TI_CC_LED2              0x02

#define TI_CC_SW_PxIN           P1IN
#define TI_CC_SW_PxIE           P1IE
#define TI_CC_SW_PxIES          P1IES
#define TI_CC_SW_PxIFG          P1IFG
#define TI_CC_SW_PxREN          P1REN
#define TI_CC_SW_PxOUT          P1OUT
#define TI_CC_SW1               BIT3

#define TI_CC_GDO0_PxOUT        P2OUT
#define TI_CC_GDO0_PxIN         P2IN
#define TI_CC_GDO0_PxDIR        P2DIR
#define TI_CC_GDO0_PxIE         P2IE
#define TI_CC_GDO0_PxIES        P2IES
#define TI_CC_GDO0_PxIFG        P2IFG
#define TI_CC_GDO0_PIN          BIT5

#define TI_CC_GDO1_PxOUT        P5OUT
#define TI_CC_GDO1_PxIN         P5IN
#define TI_CC_GDO1_PxDIR        P5DIR
#define TI_CC_GDO1_PIN          0x04

#define TI_CC_GDO2_PxOUT        P2OUT
#define TI_CC_GDO2_PxIN         P2IN
#define TI_CC_GDO2_PxDIR        P2DIR
#define TI_CC_GDO2_PIN          0x40

#define TI_CC_CSn_PxOUT         P2OUT
#define TI_CC_CSn_PxDIR         P2DIR
#define TI_CC_CSn_PIN           BIT4


//----------------------------------------------------------------------------
// Select which port will be used for interface to CCxxxx
//----------------------------------------------------------------------------
#if defined( __MSP430G2452__)
#define TI_CC_RF_SER_INTF       TI_CC_SER_INTF_USI  // Interface to CCxxxx
#elif defined( __MSP430G2533__)
#define TI_CC_RF_SER_INTF       TI_CC_SER_INTF_USCIB0  // Interface to CCxxxx
#else
#error unsupported device! (Write some code to fix support it!)
#endif


