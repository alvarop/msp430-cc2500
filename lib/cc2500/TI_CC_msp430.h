//******************************************************************************
//  Description:  This file contains definitions specific to the specific MSP430
//  chosen for this implementation.  MSP430 has multiple interfaces capable
//  of interfacing to the SPI port; each of these is defined in this file.
//
//  The source labels for the definitions (i.e., "P3SEL") can be found in
//  msp430xxxx.h.
//
//  MSP430/CC1100-2500 Interface Code Library v1.1
//
//  W. Goh
//  Texas Instruments, Inc.
//  December 2009
//  IAR Embedded Workbench v4.20
//******************************************************************************
// Change Log:
//******************************************************************************
// Version:  1.1
// Comments: Added support for various MSP430 development tools.
//           Added support for 5xx
// Version:  1.00
// Comments: Initial Release Version
//******************************************************************************

#include "msp430.h"

// SPI port definitions                     // Adjust the values for the chosen
#define TI_CC_SPI_USART0_PxSEL  P3SEL       // interfaces, according to the pin
#define TI_CC_SPI_USART0_PxDIR  P3DIR       // assignments indicated in the
#define TI_CC_SPI_USART0_PxIN   P3IN        // chosen MSP430 device datasheet.
#define TI_CC_SPI_USART0_SIMO   BIT1
#define TI_CC_SPI_USART0_SOMI   BIT2
#define TI_CC_SPI_USART0_UCLK   BIT3

// Use this setting with 4xx EXP Board
#define TI_CC_SPI_USART1_PxSEL  P4SEL
#define TI_CC_SPI_USART1_PxDIR  P4DIR
#define TI_CC_SPI_USART1_PxIN   P4IN
#define TI_CC_SPI_USART1_SIMO   BIT3
#define TI_CC_SPI_USART1_SOMI   BIT4
#define TI_CC_SPI_USART1_UCLK   BIT5

#define TI_CC_SPI_USCIA0_PxSEL  P3SEL
#define TI_CC_SPI_USCIA0_PxDIR  P3DIR
#define TI_CC_SPI_USCIA0_PxIN   P3IN
#define TI_CC_SPI_USCIA0_SIMO   BIT4
#define TI_CC_SPI_USCIA0_SOMI   BIT5
#define TI_CC_SPI_USCIA0_UCLK   BIT0

#define TI_CC_SPI_USCIA1_PxSEL  P7SEL
#define TI_CC_SPI_USCIA1_PxDIR  P7DIR
#define TI_CC_SPI_USCIA1_PxIN   P7IN
#define TI_CC_SPI_USCIA1_SIMO   0x02
#define TI_CC_SPI_USCIA1_SOMI   0x04
#define TI_CC_SPI_USCIA1_UCLK   0x08

// USCIA1 for F543x
/*
#define TI_CC_SPI_USCIA1_PxSEL  P5SEL
#define TI_CC_SPI_USCIA1_PxDIR  P5DIR
#define TI_CC_SPI_USCIA1_PxIN   P5IN
#define TI_CC_SPI_USCIA1_SIMO   BIT6
#define TI_CC_SPI_USCIA1_SOMI   BIT7
#define TI_CC_SPI_USCIA1_PxSEL_UCLK  P3SEL
#define TI_CC_SPI_USCIA1_PxDIR_UCLK  P3DIR
#define TI_CC_SPI_USCIA1_UCLK   BIT6
*/

// USCIA2 for F543x
#define TI_CC_SPI_USCIA2_PxSEL  P9SEL
#define TI_CC_SPI_USCIA2_PxDIR  P9DIR
#define TI_CC_SPI_USCIA2_PxIN   P9IN
#define TI_CC_SPI_USCIA2_SOMI   BIT5
#define TI_CC_SPI_USCIA2_UCLK   BIT0
#define TI_CC_SPI_USCIA2_SIMO   BIT4

// USCIA3 for F543x
#define TI_CC_SPI_USCIA3_PxSEL  P10SEL
#define TI_CC_SPI_USCIA3_PxDIR  P10DIR
#define TI_CC_SPI_USCIA3_PxIN   P10IN
#define TI_CC_SPI_USCIA3_SOMI   BIT5
#define TI_CC_SPI_USCIA3_UCLK   BIT0
#define TI_CC_SPI_USCIA3_SIMO   BIT4

// USCIB0 for F543x
// Use this setting for 5xx Exp Board
#define TI_CC_SPI_USCIB0_PxSEL  P3SEL
#define TI_CC_SPI_USCIB0_PxDIR  P3DIR
#define TI_CC_SPI_USCIB0_PxIN   P3IN
#define TI_CC_SPI_USCIB0_SIMO   BIT1
#define TI_CC_SPI_USCIB0_SOMI   BIT2
#define TI_CC_SPI_USCIB0_UCLK   BIT3

//USCIB0 for eZ430-RF2500
#define TI_CC_SPI_USCIB0_PxSEL  P3SEL
#define TI_CC_SPI_USCIB0_PxDIR  P3DIR
#define TI_CC_SPI_USCIB0_PxIN   P3IN
#define TI_CC_SPI_USCIB0_SIMO   BIT1
#define TI_CC_SPI_USCIB0_SOMI   BIT2
#define TI_CC_SPI_USCIB0_UCLK   BIT3

// USCIB1 for F543x
#define TI_CC_SPI_USCIB1_PxSEL  P5SEL
#define TI_CC_SPI_USCIB1_PxDIR  P5DIR
#define TI_CC_SPI_USCIB1_PxIN   P5IN
#define TI_CC_SPI_USCIB1_SOMI   BIT4
#define TI_CC_SPI_USCIB1_UCLK   BIT5
#define TI_CC_SPI_USCIB1_PxSEL_SIMO  P3SEL
#define TI_CC_SPI_USCIB1_PxDIR_SIMO  P3DIR
#define TI_CC_SPI_USCIB1_SIMO   BIT7

// USCIB2 for F543x
#define TI_CC_SPI_USCIB2_PxSEL  P9SEL
#define TI_CC_SPI_USCIB2_PxDIR  P9DIR
#define TI_CC_SPI_USCIB2_PxIN   P9IN
#define TI_CC_SPI_USCIB2_SOMI   BIT2
#define TI_CC_SPI_USCIB2_UCLK   BIT3
#define TI_CC_SPI_USCIB2_SIMO   BIT1

// USCIB3 for F543x
#define TI_CC_SPI_USCIB3_PxSEL  P10SEL
#define TI_CC_SPI_USCIB3_PxDIR  P10DIR
#define TI_CC_SPI_USCIB3_PxIN   P10IN
#define TI_CC_SPI_USCIB3_SOMI   BIT2
#define TI_CC_SPI_USCIB3_UCLK   BIT3
#define TI_CC_SPI_USCIB3_SIMO   BIT1

#define TI_CC_SPI_USI_PxDIR     P1DIR
#define TI_CC_SPI_USI_PxIN      P1IN
#define TI_CC_SPI_USI_SIMO      BIT6
#define TI_CC_SPI_USI_SOMI      BIT7
#define TI_CC_SPI_USI_UCLK      BIT5

#define TI_CC_SPI_BITBANG_PxDIR P1DIR
#define TI_CC_SPI_BITBANG_PxOUT P1OUT
#define TI_CC_SPI_BITBANG_PxIN  P1IN
#define TI_CC_SPI_BITBANG_SIMO  BIT6
#define TI_CC_SPI_BITBANG_SOMI  BIT7
#define TI_CC_SPI_BITBANG_UCLK  BIT5


//******************************************************************************
//  These constants are used to identify the chosen SPI and UART interfaces.
//******************************************************************************
#define TI_CC_SER_INTF_NULL    0
#define TI_CC_SER_INTF_USART0  1
#define TI_CC_SER_INTF_USART1  2
#define TI_CC_SER_INTF_USCIA0  3
#define TI_CC_SER_INTF_USCIA1  4
#define TI_CC_SER_INTF_USCIA2  5
#define TI_CC_SER_INTF_USCIA3  6
#define TI_CC_SER_INTF_USCIB0  7
#define TI_CC_SER_INTF_USCIB1  8
#define TI_CC_SER_INTF_USCIB2  9
#define TI_CC_SER_INTF_USCIB3  10
#define TI_CC_SER_INTF_USI     11
#define TI_CC_SER_INTF_BITBANG 12
