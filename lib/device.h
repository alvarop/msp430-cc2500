/** @file device.h
*
* @brief Include device specific functions
*
* @author Alvaro Prieto
*/
#ifndef _DEVICE_H
#define _DEVICE_H

#if defined( __MSP430G2252__)
#include "device/ti/msp430/g2252.h"

#elif defined( __MSP430G2452__)
#include "device/ti/msp430/g2452.h"

#elif defined( __MSP430G2412__)
#include "device/ti/msp430/g2412.h"

#elif defined( __MSP430G2203__)
#include "device/ti/msp430/g2203.h"

#elif defined( __MSP430G2533__)
#include "device/ti/msp430/g2533.h"

#elif defined( __MSP430G2553__)
#include "device/ti/msp430/g2553.h"

#else
#error Device not supported. Create your <device>.h file under lib/device/ and add it here!
#endif

#endif /* _DEVICE_H */



