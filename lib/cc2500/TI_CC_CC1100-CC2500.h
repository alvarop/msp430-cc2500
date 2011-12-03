//----------------------------------------------------------------------------
//  Description:  This file contains definitions specific to the CC1100/2500.
//  The configuration registers, strobe commands, and status registers are 
//  defined, as well as some common masks for these registers.
//
//  MSP430/CC1100-2500 Interface Code Library v1.0
//
//  K. Quiring
//  Texas Instruments, Inc.
//  July 2006
//  IAR Embedded Workbench v3.41
//----------------------------------------------------------------------------


// Configuration Registers
#define TI_CCxxx0_IOCFG2       0x00        // GDO2 output pin configuration
#define TI_CCxxx0_IOCFG1       0x01        // GDO1 output pin configuration
#define TI_CCxxx0_IOCFG0       0x02        // GDO0 output pin configuration
#define TI_CCxxx0_FIFOTHR      0x03        // RX FIFO and TX FIFO thresholds
#define TI_CCxxx0_SYNC1        0x04        // Sync word, high byte
#define TI_CCxxx0_SYNC0        0x05        // Sync word, low byte
#define TI_CCxxx0_PKTLEN       0x06        // Packet length
#define TI_CCxxx0_PKTCTRL1     0x07        // Packet automation control
#define TI_CCxxx0_PKTCTRL0     0x08        // Packet automation control
#define TI_CCxxx0_ADDR         0x09        // Device address
#define TI_CCxxx0_CHANNR       0x0A        // Channel number
#define TI_CCxxx0_FSCTRL1      0x0B        // Frequency synthesizer control
#define TI_CCxxx0_FSCTRL0      0x0C        // Frequency synthesizer control
#define TI_CCxxx0_FREQ2        0x0D        // Frequency control word, high byte
#define TI_CCxxx0_FREQ1        0x0E        // Frequency control word, middle byte
#define TI_CCxxx0_FREQ0        0x0F        // Frequency control word, low byte
#define TI_CCxxx0_MDMCFG4      0x10        // Modem configuration
#define TI_CCxxx0_MDMCFG3      0x11        // Modem configuration
#define TI_CCxxx0_MDMCFG2      0x12        // Modem configuration
#define TI_CCxxx0_MDMCFG1      0x13        // Modem configuration
#define TI_CCxxx0_MDMCFG0      0x14        // Modem configuration
#define TI_CCxxx0_DEVIATN      0x15        // Modem deviation setting
#define TI_CCxxx0_MCSM2        0x16        // Main Radio Cntrl State Machine config
#define TI_CCxxx0_MCSM1        0x17        // Main Radio Cntrl State Machine config
#define TI_CCxxx0_MCSM0        0x18        // Main Radio Cntrl State Machine config
#define TI_CCxxx0_FOCCFG       0x19        // Frequency Offset Compensation config
#define TI_CCxxx0_BSCFG        0x1A        // Bit Synchronization configuration
#define TI_CCxxx0_AGCCTRL2     0x1B        // AGC control
#define TI_CCxxx0_AGCCTRL1     0x1C        // AGC control
#define TI_CCxxx0_AGCCTRL0     0x1D        // AGC control
#define TI_CCxxx0_WOREVT1      0x1E        // High byte Event 0 timeout
#define TI_CCxxx0_WOREVT0      0x1F        // Low byte Event 0 timeout
#define TI_CCxxx0_WORCTRL      0x20        // Wake On Radio control
#define TI_CCxxx0_FREND1       0x21        // Front end RX configuration
#define TI_CCxxx0_FREND0       0x22        // Front end TX configuration
#define TI_CCxxx0_FSCAL3       0x23        // Frequency synthesizer calibration
#define TI_CCxxx0_FSCAL2       0x24        // Frequency synthesizer calibration
#define TI_CCxxx0_FSCAL1       0x25        // Frequency synthesizer calibration
#define TI_CCxxx0_FSCAL0       0x26        // Frequency synthesizer calibration
#define TI_CCxxx0_RCCTRL1      0x27        // RC oscillator configuration
#define TI_CCxxx0_RCCTRL0      0x28        // RC oscillator configuration
#define TI_CCxxx0_FSTEST       0x29        // Frequency synthesizer cal control
#define TI_CCxxx0_PTEST        0x2A        // Production test
#define TI_CCxxx0_AGCTEST      0x2B        // AGC test
#define TI_CCxxx0_TEST2        0x2C        // Various test settings
#define TI_CCxxx0_TEST1        0x2D        // Various test settings
#define TI_CCxxx0_TEST0        0x2E        // Various test settings

// Strobe commands
#define TI_CCxxx0_SRES         0x30        // Reset chip.
#define TI_CCxxx0_SFSTXON      0x31        // Enable/calibrate freq synthesizer
#define TI_CCxxx0_SXOFF        0x32        // Turn off crystal oscillator.
#define TI_CCxxx0_SCAL         0x33        // Calibrate freq synthesizer & disable
#define TI_CCxxx0_SRX          0x34        // Enable RX.
#define TI_CCxxx0_STX          0x35        // Enable TX.
#define TI_CCxxx0_SIDLE        0x36        // Exit RX / TX
#define TI_CCxxx0_SAFC         0x37        // AFC adjustment of freq synthesizer
#define TI_CCxxx0_SWOR         0x38        // Start automatic RX polling sequence
#define TI_CCxxx0_SPWD         0x39        // Enter pwr down mode when CSn goes hi
#define TI_CCxxx0_SFRX         0x3A        // Flush the RX FIFO buffer.
#define TI_CCxxx0_SFTX         0x3B        // Flush the TX FIFO buffer.
#define TI_CCxxx0_SWORRST      0x3C        // Reset real time clock.
#define TI_CCxxx0_SNOP         0x3D        // No operation.

// Status registers
#define TI_CCxxx0_PARTNUM      0x30        // Part number
#define TI_CCxxx0_VERSION      0x31        // Current version number
#define TI_CCxxx0_FREQEST      0x32        // Frequency offset estimate
#define TI_CCxxx0_LQI          0x33        // Demodulator estimate for link quality
#define TI_CCxxx0_RSSI         0x34        // Received signal strength indication
#define TI_CCxxx0_MARCSTATE    0x35        // Control state machine state
#define TI_CCxxx0_WORTIME1     0x36        // High byte of WOR timer
#define TI_CCxxx0_WORTIME0     0x37        // Low byte of WOR timer
#define TI_CCxxx0_PKTSTATUS    0x38        // Current GDOx status and packet status
#define TI_CCxxx0_VCO_VC_DAC   0x39        // Current setting from PLL cal module
#define TI_CCxxx0_TXBYTES      0x3A        // Underflow and # of bytes in TXFIFO
#define TI_CCxxx0_RXBYTES      0x3B        // Overflow and # of bytes in RXFIFO
#define TI_CCxxx0_NUM_RXBYTES  0x7F        // Mask "# of bytes" field in _RXBYTES

// Other memory locations
#define TI_CCxxx0_PATABLE      0x3E
#define TI_CCxxx0_TXFIFO       0x3F
#define TI_CCxxx0_RXFIFO       0x3F

// Masks for appended status bytes
#define TI_CCxxx0_LQI_RX       0x01        // Position of LQI byte
#define TI_CCxxx0_CRC_OK       0x80        // Mask "CRC_OK" bit within LQI byte

// Definitions to support burst/single access:
#define TI_CCxxx0_WRITE_BURST  0x40
#define TI_CCxxx0_READ_SINGLE  0x80
#define TI_CCxxx0_READ_BURST   0xC0


