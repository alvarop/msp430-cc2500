/** @file uart.h
*
* @brief UART functions
*
* @author Alvaro Prieto
*/
#ifndef _UART_H
#define _UART_H

#include <stdint.h>

#define ESCAPE_BYTE 0x7D
#define START_BYTE 0x7E
#define END_BYTE 0x7F

void setup_uart( void );

void uart_put_char( uint8_t );

void uart_write( uint8_t*, uint16_t );

void uart_write_escaped( uint8_t*, uint16_t );

void setup_uart_callback( uint8_t (*)(uint8_t) );

#endif /* _UART_H */
