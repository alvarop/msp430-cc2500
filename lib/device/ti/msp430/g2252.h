/** @file g2452.h
*
* @brief Hardware Definitions for msp430g2452
*
* @author Alvaro Prieto
*/
#ifndef _G2452_H
#define _G2452_H

#if !defined( __MSP430G2252__)
#error This header file is for the MSP430G2252 device!
#endif

#include "msp430g2252.h"

// LED port and pin definitions
#define LED_PxOUT         P1OUT
#define LED_PxDIR         P1DIR
#define LED1              BIT0
#define LED2              BIT3

// GDO0 Port and pin definitions
// Must be an input with interrupts
#define GDO0_PxOUT        P2OUT
#define GDO0_PxIN         P2IN
#define GDO0_PxDIR        P2DIR
#define GDO0_PxIE         P2IE
#define GDO0_PxIES        P2IES
#define GDO0_PxIFG        P2IFG
#define GDO0_PIN          BIT5

// CSn Port and pin definitions
#define CSn_PxOUT         P2OUT
#define CSn_PxDIR         P2DIR
#define CSn_PIN           BIT4

// SPI Pin definitions
#define SPI_USI_PxDIR     P1DIR
#define SPI_USI_PxIN      P1IN
#define SPI_USI_SIMO      BIT6
#define SPI_USI_SOMI      BIT7
#define SPI_USI_UCLK      BIT5

// Make sure we use the correct SPI interface
#define SPI_INTERFACE_USI

#endif /* _G2452_H */
