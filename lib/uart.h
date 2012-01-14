/** @file uart.h
*
* @brief UART functions
*
* @author Alvaro Prieto
*/
#ifndef _UART_H
#define _UART_H

#include <stdint.h>
#include <msp430.h>

#define CSn_PxOUT       P3OUT
#define CSn_PxDIR       P3DIR
#define CSn_PIN         BIT0

#define SYNC_BYTE 0x7E
#define ESCAPE_BYTE 0x7D

void setup_uart( void );

void uart_put_char( uint8_t );

void uart_write( uint8_t*, uint16_t );

void uart_write_escaped( uint8_t*, uint16_t );

void setup_uart_callback( uint8_t (*)(uint8_t) );

uint8_t hex_to_string( uint8_t* , uint8_t*, uint8_t );

#endif /* _UART_H */
