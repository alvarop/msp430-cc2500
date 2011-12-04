/** @file radio_cc2500.c
*
* @brief CC2500 radio functions
*
* @author Alvaro Prieto
*/
#include "radio_cc2500.h"
#include "TI_CC_include.h"
#include <signal.h>
#include <string.h>

// Define positions in buffer for various fields
#define LENGTH_FIELD  (0)
#define ADDRESS_FIELD (1)
#define DATA_FIELD    (2)

extern uint8_t paTable[];
extern uint8_t paTableLen;

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
  //uint8_t max_power = 0xff;

  // Set-up rx_callback function
  rx_callback = callback;
  
  TI_CC_SPISetup();                         // Initialize SPI port

  TI_CC_PowerupResetCCxxxx();               // Reset CCxxxx
  writeRFSettings();                        // Write RF settings to config reg
  TI_CC_SPIWriteBurstReg(TI_CCxxx0_PATABLE, paTable, paTableLen);//Write PATABLE
 
  TI_CC_SPIStrobe(TI_CCxxx0_SRX);           // Initialize CCxxxx in RX mode.
                                            // When a pkt is received, it will
                                            // signal on GDO0 and wake CPU  
                                            
  // Configure GDO0 port                                        
  TI_CC_GDO0_PxIES |= TI_CC_GDO0_PIN;       // Int on falling edge (end of pkt)
  TI_CC_GDO0_PxIFG &= ~TI_CC_GDO0_PIN;      // Clear flag
  TI_CC_GDO0_PxIE |= TI_CC_GDO0_PIN;        // Enable int on end of packet

}

/*******************************************************************************
 * @fn     cc2500_tx( uint8_t* p_buffer, uint8_t length )
 * @brief  Send raw message through radio
 * ****************************************************************************/
void cc2500_tx( uint8_t* p_buffer, uint8_t length )
{ 
  TI_CC_GDO0_PxIE &= ~TI_CC_GDO0_PIN;          // Disable interrupt
  
  TI_CC_SPIWriteBurstReg(TI_CCxxx0_TXFIFO, p_buffer, length); // Write TX data
  TI_CC_SPIStrobe(TI_CCxxx0_STX);           // Change state to TX, initiating
                                            // data transfer

  while (!(TI_CC_GDO0_PxIN&TI_CC_GDO0_PIN));
                                            // Wait GDO0 to go hi -> sync TX'ed
  while (TI_CC_GDO0_PxIN&TI_CC_GDO0_PIN);
                                            // Wait GDO0 to clear -> end of pkt
  TI_CC_GDO0_PxIFG &= ~TI_CC_GDO0_PIN;      // After pkt TX, this flag is set.
                                            // Has to be cleared before existing
  
  TI_CC_GDO0_PxIFG &= ~TI_CC_GDO0_PIN;          // Clear flag
  TI_CC_GDO0_PxIE |= TI_CC_GDO0_PIN;            // Enable interrupt
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
  TI_CC_SPIWriteReg( TI_CCxxx0_ADDR, address );
}

/*******************************************************************************
 * @fn     cc2500_set_channel( uint8_t );
 * @brief  Set device channel
 * ****************************************************************************/
void cc2500_set_channel( uint8_t channel )
{
  TI_CC_SPIWriteReg( TI_CCxxx0_CHANNR, channel );
}

/*******************************************************************************
 * @fn     cc2500_set_power( uint8_t );
 * @brief  Set device transmit power
 * ****************************************************************************/
void cc2500_set_power( uint8_t power )
{  
  // Set TX power
  TI_CC_SPIWriteBurstReg(TI_CCxxx0_PATABLE, &power, 1 );
}

/*******************************************************************************
 * @fn     cc2500_enable_addressing( );
 * @brief  Enable address checking with 0x00 as a broadcast address
 * ****************************************************************************/
void cc2500_enable_addressing()
{  
  uint8_t tmp_reg;

  tmp_reg = ( TI_CC_SPIReadReg( TI_CCxxx0_PKTCTRL1  ) & ~0x03 ) | 0x02;

  TI_CC_SPIWriteReg( TI_CCxxx0_PKTCTRL1, tmp_reg );
}

/*******************************************************************************
 * @fn     cc2500_disable_addressing( );
 * @brief  Disable address checking
 * ****************************************************************************/
void cc2500_disable_addressing()
{  
  uint8_t tmp_reg;

  tmp_reg = ( TI_CC_SPIReadReg( TI_CCxxx0_PKTCTRL1  ) & ~0x03 );

  TI_CC_SPIWriteReg( TI_CCxxx0_PKTCTRL1, tmp_reg );
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
  if ( ( TI_CC_SPIReadStatus( TI_CCxxx0_RXBYTES ) & TI_CCxxx0_NUM_RXBYTES ) )
  {
    // Read the first byte which contains the packet length
    packet_length = TI_CC_SPIReadReg( TI_CCxxx0_RXFIFO );

    // Make sure the packet length is smaller than our buffer
    if ( packet_length <= *length )
    {
      // Read the rest of the packet
      TI_CC_SPIReadBurstReg( TI_CCxxx0_RXFIFO, p_buffer, packet_length );
      
      // Return packet size in length variable
      *length = packet_length;
      
      // Read two byte status 
      TI_CC_SPIReadBurstReg( TI_CCxxx0_RXFIFO, status, 2 );
      
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
      TI_CC_SPIStrobe(TI_CCxxx0_SFRX);      // Flush RXFIFO
      
      return 0;
    }
    
  }
 
  return 0;  
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
  if ( TI_CC_GDO0_PxIFG & TI_CC_GDO0_PIN )
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
  TI_CC_GDO0_PxIFG &= ~TI_CC_GDO0_PIN;  // Clear interrupt flag
  
  // Only needed if radio is configured to return to IDLE after transmission
  // Check register MCSM1.TXOFF_MODE
  //strobe( SRX ); // enter receive mode again
}
