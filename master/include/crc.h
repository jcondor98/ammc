// AVR Multi Motor Control
/*! \file master/include/crc.h
 * Packet-switched communication layer
 *
 * \author Paolo Lucchesi
 */
#ifndef __CRC_MODULE_H
#define __CRC_MODULE_H
#include <stdint.h>

// CRC Algorithm parameters
// The standard CRC-8 is used, but any other algorithm which does not make use
// of data reflection or xorout is supported

//! Name of the CRC algorithm
#define CRC_NAME "CRC-8"

//! CRC polynomial used
#define CRC_POLY 0x07

//! CRC initial value
#define CRC_INIT 0x00

#define CRC_CHECK 0xF4

//! CRC type definition
typedef uint8_t crc_t;


/*!
 * Compute the CRC of a chunk of data
 *
 * Do not include trailing zeroes as a room for the CRC (if there are, then
 * truncate the 'size' parameter so they are not counted)
 *
 * @param data Pointer to the data memory area
 * @param size Size of the data parameter in bytes
 * @returns The computed CRC value
 */
crc_t crc(const void *data, uint8_t size);

/*!
 * Check a chunk of data with a trailing CRC
 *
 * You have to count the bytes taken by the CRC in the 'size' parameter
 * It performs a division of the entire message by the polynomial.
 * If there is no remainder, the message should not be corrupted
 * (at least by a detectable error).
 *
 * @param data Pointer to the data memory area
 * @param size Size of the data parameter in bytes
 * @return The remainder of the division
 */
crc_t crc_check(const void *data, uint8_t size);

#endif  // __CRC_MODULE_H
