/** @file f2274.h
*
* @brief Hardware Definitions for msp430f2274
*
* @author Alvaro Prieto
*/
#ifndef _F2274_H
#define _F2274_H

#if !defined( __MSP430F2274__)
#error This header file is for the MSP430f2274 device!
#endif

#include "msp430f2274.h"

#define LED_PxOUT         P1OUT
#define LED_PxDIR         P1DIR
#define LED1              BIT0
#define LED2              BIT1

#define GDO0_PxOUT        P2OUT
#define GDO0_PxIN         P2IN
#define GDO0_PxDIR        P2DIR
#define GDO0_PxIE         P2IE
#define GDO0_PxIES        P2IES
#define GDO0_PxIFG        P2IFG
#define GDO0_PIN          BIT6

#define CSn_PxOUT         P3OUT
#define CSn_PxDIR         P3DIR
#define CSn_PIN           BIT0

// SPI Pin Definitions
#define SPI_USCIB0_PxSEL  P3SEL
#define SPI_USCIB0_PxSEL2 P3SEL2
#define SPI_USCIB0_PxDIR  P3DIR
#define SPI_USCIB0_PxIN   P3IN
#define SPI_USCIB0_SIMO   BIT1
#define SPI_USCIB0_SOMI   BIT2
#define SPI_USCIB0_UCLK   BIT3

// Make sure we use the correct UART interface
#define UART_INTERFACE_USCIA0

// Make sure we use the correct SPI interface
#define SPI_INTERFACE_USCIB0

#endif /* _f2274_H */
