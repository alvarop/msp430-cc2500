/** @file cc2500.c
*
* @brief CC2500 radio functions 
*
* @author Alvaro Prieto
*/
#include "cc2500.h"
#include "device.h"
#include "spi.h"
#include <string.h>

// Define positions in buffer for various fields
#define LENGTH_FIELD  (0)
#define ADDRESS_FIELD (1)
#define DATA_FIELD    (2)

static uint8_t dummy_callback( uint8_t*, uint8_t );
uint8_t receive_packet( uint8_t*, uint8_t* );

// Receive buffer
static uint8_t p_rx_buffer[CC2500_BUFFER_LENGTH];
static uint8_t p_tx_buffer[CC2500_BUFFER_LENGTH];

// Holds pointers to all callback functions for CCR registers (and overflow)
static uint8_t (*rx_callback)( uint8_t*, uint8_t ) = dummy_callback;

//
// Optimum PATABLE levels according to Table 31 on CC2500 datasheet
//
/*static const uint8_t power_table[] = {
                              0x00, 0x50, 0x44, 0xC0, // -55, -30, -28, -26 dBm
                              0x84, 0x81, 0x46, 0x93, // -24, -22, -20, -18 dBm
                              0x55, 0x8D, 0xC6, 0x97, // -16, -14, -12, -10 dBm
                              0x6E, 0x7F, 0xA9, 0xBB, // -8,  -6,  -4,  -2  dBm
                              0xFE, 0xFF };           //  0,   1            dBm
*/

/*******************************************************************************
 * @fn     void setup_radio( uint8_t (*callback)(void) )
 * @brief  Initialize radio and register Rx Callback function
 * ****************************************************************************/
void setup_cc2500( uint8_t (*callback)(uint8_t*, uint8_t) )
{
  uint8_t initial_power = 0xFB;				// 0 dBm

  // Set-up rx_callback function
  rx_callback = callback;

  spi_setup();                         // Initialize SPI port

  cc_powerup_reset();               // Reset CCxxxx

  wait_cycles(500);  // Wait for device to reset (Not sure why this is needed)

  writeRFSettings();                        // Write RF settings to config reg
  cc_write_burst_reg( TI_CCxxx0_PATABLE, &initial_power, 1);//Write PATABLE

  cc_strobe(TI_CCxxx0_SRX);           // Initialize CCxxxx in RX mode.
                                            // When a pkt is received, it will
                                            // signal on GDO0 and wake CPU

  // Configure GDO0 port
  GDO0_PxIES |= GDO0_PIN;       // Int on falling edge (end of pkt)
  GDO0_PxIFG &= ~GDO0_PIN;      // Clear flag
  GDO0_PxIE |= GDO0_PIN;        // Enable int on end of packet

}

/*******************************************************************************
 * @fn     cc2500_tx( uint8_t* p_buffer, uint8_t length )
 * @brief  Send raw message through radio
 * ****************************************************************************/
void cc2500_tx( uint8_t* p_buffer, uint8_t length )
{
  GDO0_PxIE &= ~GDO0_PIN;          // Disable interrupt

  cc_write_burst_reg(TI_CCxxx0_TXFIFO, p_buffer, length); // Write TX data
  cc_strobe(TI_CCxxx0_STX);           // Change state to TX, initiating
                                            // data transfer

  while (!(GDO0_PxIN&GDO0_PIN));
                                            // Wait GDO0 to go hi -> sync TX'ed
  while (GDO0_PxIN&GDO0_PIN);
                                            // Wait GDO0 to clear -> end of pkt
  GDO0_PxIFG &= ~GDO0_PIN;      // After pkt TX, this flag is set.
                                            // Has to be cleared before existing

  GDO0_PxIFG &= ~GDO0_PIN;          // Clear flag
  GDO0_PxIE |= GDO0_PIN;            // Enable interrupt
}

/*******************************************************************************
 * @fn     void cc2500_tx_packet( uint8_t* p_buffer, uint8_t length,
 *                                                        uint8_t destination )
 * @brief  Send packet through radio. Takes care of adding length and
 *         destination to packet.
 * ****************************************************************************/
void cc2500_tx_packet( uint8_t* p_buffer, uint8_t length, uint8_t destination )
{
  // Add one to packet length account for address byte
  p_tx_buffer[LENGTH_FIELD] = length + 1;

  // Insert destination address to buffer
  p_tx_buffer[ADDRESS_FIELD] = destination;

  // Copy message buffer into tx buffer. Add one to account for length byte
  memcpy( &p_tx_buffer[DATA_FIELD], p_buffer, length );

  // Add DATA_FIELD to account for packet length and address bytes
  cc2500_tx( p_tx_buffer, (length + DATA_FIELD) );
}

/*******************************************************************************
 * @fn     cc2500_set_address( uint8_t );
 * @brief  Set device address
 * ****************************************************************************/
void cc2500_set_address( uint8_t address )
{
  cc_write_reg( TI_CCxxx0_ADDR, address );
}

/*******************************************************************************
 * @fn     cc2500_set_channel( uint8_t );
 * @brief  Set device channel
 * ****************************************************************************/
void cc2500_set_channel( uint8_t channel )
{
  cc_write_reg( TI_CCxxx0_CHANNR, channel );
}

/*******************************************************************************
 * @fn     cc2500_set_power( uint8_t );
 * @brief  Set device transmit power
 * ****************************************************************************/
void cc2500_set_power( uint8_t power )
{
  // Set TX power
  cc_write_burst_reg(TI_CCxxx0_PATABLE, &power, 1 );
}

/*******************************************************************************
 * @fn     cc2500_enable_addressing( );
 * @brief  Enable address checking with 0x00 as a broadcast address
 * ****************************************************************************/
void cc2500_enable_addressing()
{
  uint8_t tmp_reg;

  tmp_reg = ( cc_read_reg( TI_CCxxx0_PKTCTRL1  ) & ~0x03 ) | 0x02;

  cc_write_reg( TI_CCxxx0_PKTCTRL1, tmp_reg );
}

/*******************************************************************************
 * @fn     cc2500_disable_addressing( );
 * @brief  Disable address checking
 * ****************************************************************************/
void cc2500_disable_addressing()
{
  uint8_t tmp_reg;

  tmp_reg = ( cc_read_reg( TI_CCxxx0_PKTCTRL1  ) & ~0x03 );

  cc_write_reg( TI_CCxxx0_PKTCTRL1, tmp_reg );
}

/*******************************************************************************
 * @fn     cc2500_sleep( );
 * @brief  Set device to low power sleep mode
 * ****************************************************************************/
void cc2500_sleep( )
{
  // Set device to idle
  cc_strobe(TI_CCxxx0_SIDLE);

  // Set device to power-down (sleep) mode
  cc_strobe(TI_CCxxx0_SPWD);
}

/*******************************************************************************
 * @fn     void dummy_callback( void )
 * @brief  empty function works as default callback
 * ****************************************************************************/
static uint8_t dummy_callback( uint8_t* buffer, uint8_t length )
{
  __no_operation();

  return 0;
}

/*******************************************************************************
 * @fn     uint8_t receive_packet( uint8_t* p_buffer, uint8_t* length )
 * @brief  Receive packet from the radio using CC2500
 * ****************************************************************************/
uint8_t receive_packet( uint8_t* p_buffer, uint8_t* length )
{
  uint8_t status[2];
  uint8_t packet_length;

  // Make sure there are bytes to be read in the FIFO buffer
  if ( ( cc_read_status( TI_CCxxx0_RXBYTES ) & TI_CCxxx0_NUM_RXBYTES ) )
  {
    // Read the first byte which contains the packet length
    packet_length = cc_read_reg( TI_CCxxx0_RXFIFO );

    // Make sure the packet length is smaller than our buffer
    if ( packet_length <= *length )
    {
      // Read the rest of the packet
      cc_read_burst_reg( TI_CCxxx0_RXFIFO, p_buffer, packet_length );

      // Return packet size in length variable
      *length = packet_length;

      // Read two byte status
      cc_read_burst_reg( TI_CCxxx0_RXFIFO, status, 2 );

      // Append status bytes to buffer
      memcpy( &p_buffer[packet_length], status, 2 );

      // Return 1 when CRC matches, 0 otherwise
      return ( status[TI_CCxxx0_LQI_RX] & TI_CCxxx0_CRC_OK );
    }
    else
    {
      // If the packet is larger than the buffer, flush the RX FIFO
      *length = packet_length;

      // Flush RX FIFO
      cc_strobe(TI_CCxxx0_SFRX);      // Flush RXFIFO

      return 0;
    }

  }

  return 0;
}

// Product = CC2500
// Crystal accuracy = 40 ppm
// X-tal frequency = 26 MHz
// RF output power = 0 dBm
// RX filterbandwidth = 540.000000 kHz
// Deviation = 0.000000
// Return state:  Return to RX state upon leaving either TX or RX
// Datarate = 250.000000 kbps
// Modulation = (7) MSK
// Manchester enable = (0) Manchester disabled
// RF Frequency = 2433.000000 MHz
// Channel spacing = 199.950000 kHz
// Channel number = 0
// Optimization = Sensitivity
// Sync mode = (3) 30/32 sync word bits detected
// Format of RX/TX data = (0) Normal mode, use FIFOs for RX and TX
// CRC operation = (1) CRC calculation in TX and CRC check in RX enabled
// Forward Error Correction = (0) FEC disabled
// Length configuration = (1) Variable length packets, packet length configured by the first received byte after sync word.
// Packetlength = 255
// Preamble count = (2)  4 bytes
// Append status = 1
// Address check = Address check and 0 (0x00) broadcast
// CRC autoflush = true
// Device address = 1
// GDO0 signal selection = ( 0x06 ) Asserts when sync word has been sent / received, and de-asserts at the end of the packet
// GDO2 signal selection = ( 0x0E ) Carrier sense. High if RSSI level is above threshold.
void writeRFSettings(void)
{

  // Write register settings
  cc_write_reg(TI_CCxxx0_IOCFG2,   0x0E);  // GDO2 output pin config.
  cc_write_reg(TI_CCxxx0_IOCFG0,   0x06);  // GDO0 output pin config.
  cc_write_reg(TI_CCxxx0_PKTLEN,   0x3D);  // Packet length.
  cc_write_reg(TI_CCxxx0_PKTCTRL1, 0x0E);  // Packet automation control.
  cc_write_reg(TI_CCxxx0_PKTCTRL0, 0x05);  // Packet automation control.
  cc_write_reg(TI_CCxxx0_ADDR,     0x01);  // Device address.
  cc_write_reg(TI_CCxxx0_CHANNR,   0x00); // Channel number.
  cc_write_reg(TI_CCxxx0_FSCTRL1,  0x07); // Freq synthesizer control.
  cc_write_reg(TI_CCxxx0_FSCTRL0,  0x00); // Freq synthesizer control.
  cc_write_reg(TI_CCxxx0_FREQ2,    0x5D); // Freq control word, high byte
  cc_write_reg(TI_CCxxx0_FREQ1,    0x93); // Freq control word, mid byte.
  cc_write_reg(TI_CCxxx0_FREQ0,    0xB1); // Freq control word, low byte.
  cc_write_reg(TI_CCxxx0_MDMCFG4,  0x2D); // Modem configuration.
  cc_write_reg(TI_CCxxx0_MDMCFG3,  0x3B); // Modem configuration.
  cc_write_reg(TI_CCxxx0_MDMCFG2,  0x73); // Modem configuration.
  cc_write_reg(TI_CCxxx0_MDMCFG1,  0x22); // Modem configuration.
  cc_write_reg(TI_CCxxx0_MDMCFG0,  0xF8); // Modem configuration.
  cc_write_reg(TI_CCxxx0_DEVIATN,  0x00); // Modem dev (when FSK mod en)
  cc_write_reg(TI_CCxxx0_MCSM1 ,   0x2F); //MainRadio Cntrl State Machine
  cc_write_reg(TI_CCxxx0_MCSM0 ,   0x18); //MainRadio Cntrl State Machine
  cc_write_reg(TI_CCxxx0_FOCCFG,   0x1D); // Freq Offset Compens. Config
  cc_write_reg(TI_CCxxx0_BSCFG,    0x1C); //  Bit synchronization config.
  cc_write_reg(TI_CCxxx0_AGCCTRL2, 0xC7); // AGC control.
  cc_write_reg(TI_CCxxx0_AGCCTRL1, 0x00); // AGC control.
  cc_write_reg(TI_CCxxx0_AGCCTRL0, 0xB2); // AGC control.
  cc_write_reg(TI_CCxxx0_FREND1,   0xB6); // Front end RX configuration.
  cc_write_reg(TI_CCxxx0_FREND0,   0x10); // Front end RX configuration.
  cc_write_reg(TI_CCxxx0_FSCAL3,   0xEA); // Frequency synthesizer cal.
  cc_write_reg(TI_CCxxx0_FSCAL2,   0x0A); // Frequency synthesizer cal.
  cc_write_reg(TI_CCxxx0_FSCAL1,   0x00); // Frequency synthesizer cal.
  cc_write_reg(TI_CCxxx0_FSCAL0,   0x11); // Frequency synthesizer cal.
  cc_write_reg(TI_CCxxx0_FSTEST,   0x59); // Frequency synthesizer cal.
  cc_write_reg(TI_CCxxx0_TEST2,    0x88); // Various test settings.
  cc_write_reg(TI_CCxxx0_TEST1,    0x31); // Various test settings.
  cc_write_reg(TI_CCxxx0_TEST0,    0x0B);  // Various test settings.
}

/*******************************************************************************
 * @fn     void port2_isr( void )
 * @brief  SPI ISR (NOTE: Port must be the same as GDO0 port!)
 * ****************************************************************************/
#pragma vector=PORT2_VECTOR
__interrupt void port2_isr(void) // CHANGE
{
  uint8_t length = CC2500_BUFFER_LENGTH;

  // Check to see if this interrupt was caused by the GDO0 pin from the CC2500
  if ( GDO0_PxIFG & GDO0_PIN )
  {
      if( receive_packet(p_rx_buffer,&length) )
      {
        // Successful packet receive, now send data to callback function
        if( rx_callback( p_rx_buffer, length ) )
        {
          // If rx_callback returns nonzero, wakeup the processor
          __bic_SR_register_on_exit(LPM1_bits);
        }

        // Clear the buffer
        memset( p_rx_buffer, 0x00, sizeof(p_rx_buffer) );
      }
      else
      {
        // A failed receive can occur due to bad CRC or (if address checking is
        // enabled) an address mismatch

        //uart_write("CRC NOK\r\n", 9);
      }

  }
  GDO0_PxIFG &= ~GDO0_PIN;  // Clear interrupt flag

  // Only needed if radio is configured to return to IDLE after transmission
  // Check register MCSM1.TXOFF_MODE
  // cc_strobe(TI_CCxxx0_SRX); // enter receive mode again
}
