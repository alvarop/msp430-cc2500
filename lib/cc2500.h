/** @file cc2500.h
*
* @brief cc2500 radio functions
*
* @author Alvaro Prieto
*     derived from SLAA465 examples
*/
#ifndef _CC2500_H
#define _CC2500_H

#include <stdint.h>

#define CC2500_BUFFER_LENGTH 64

#ifndef DEVICE_ADDRESS
#define DEVICE_ADDRESS 0x00
#error Device address not set!
#endif

void setup_cc2500( uint8_t (*)(uint8_t*, uint8_t) );
void cc2500_tx( uint8_t*, uint8_t );

void cc2500_tx_packet( uint8_t*, uint8_t, uint8_t );

void cc2500_set_address( uint8_t );
void cc2500_set_channel( uint8_t );
void cc2500_set_power( uint8_t );

void cc2500_sleep( );

void cc2500_enable_addressing();
void cc2500_disable_addressing();

void writeRFSettings(void);

/**
 * Packet header structure. The packet length byte is omitted, since the receive
 * function strips it away. Also, the cc2500_tx_packet function inserts it
 * automatically.
 */
typedef struct
{
  uint8_t destination;  // Packet destination
  uint8_t source;       // Packet source
  uint8_t type;         // Packet Type
  uint8_t flags;        // Misc flags
} packet_header_t;

//
// Packet Type Definitions (todo)
//
#define IO_CHANGE (0x01)


#endif /* _CC2500_H */
