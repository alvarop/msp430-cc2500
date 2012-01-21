/** @file device.h
*
* @brief Include device specific functions
*
* @author Alvaro Prieto
*/
#ifndef _DEVICE_H
#define _DEVICE_H

#if defined( __MSP430G2452__)
#include "device/ti/msp430/g2452.h"

#elif defined( __MSP430G2533__)
#include "device/ti/msp430/g2533.h"

#else
#error Device not supported. Create your <device>.h file under lib/device/ and add it here!
#endif

#endif /* _DEVICE_H */



