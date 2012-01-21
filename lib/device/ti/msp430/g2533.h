/** @file g2533.h
*
* @brief Hardware Definitions for msp430g2533
*
* @author Alvaro Prieto
*/
#ifndef _G2533_H
#define _G2533_H

#if !defined( __MSP430G2533__)
#error This header file is for the MSP430G2533 device!
#endif

#include "msp430g2533.h"

#define LED_PxOUT         P1OUT
#define LED_PxDIR         P1DIR
#define LED1              BIT0
#define LED2              BIT3

#define GDO0_PxOUT        P2OUT
#define GDO0_PxIN         P2IN
#define GDO0_PxDIR        P2DIR
#define GDO0_PxIE         P2IE
#define GDO0_PxIES        P2IES
#define GDO0_PxIFG        P2IFG
#define GDO0_PIN          BIT5

#define CSn_PxOUT         P2OUT
#define CSn_PxDIR         P2DIR
#define CSn_PIN           BIT4

// SPI Pin Definitions
#define SPI_USCIB0_PxSEL  P1SEL
#define SPI_USCIB0_PxSEL2 P1SEL2
#define SPI_USCIB0_PxDIR  P1DIR
#define SPI_USCIB0_PxIN   P1IN
#define SPI_USCIB0_SIMO   BIT7
#define SPI_USCIB0_SOMI   BIT6
#define SPI_USCIB0_UCLK   BIT5

// Make sure we use the correct UART interface
#define UART_INTERFACE_USCIA0

// Make sure we use the correct SPI interface
#define SPI_INTERFACE_USCIB0

#endif /* _G2533_H */
