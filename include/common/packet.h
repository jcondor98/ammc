// AVR Multi Motor Control
/*! \file include/common/packet.h
 * Packet module common definitions
 *
 * \author Paolo Lucchesi
 */
#ifndef __PACKET_MODULE_COMMON_H
#define __PACKET_MODULE_COMMON_H
#include <stdint.h>
#include "crc.h"

#define BODY_MAX_LEN 32
#define PACKET_MIN_SIZE (sizeof(header_t) + sizeof(crc_t))

/*!
 * Type definition for a whole packet
 *
 * The header is structured as follows:
 *   unsigned id       : 8
 *   unsigned type     : 8
 *   unsigned selector : 8
 *   unsigned size     : 8
 * Selector is ignored on types different than GET_SPEED and SET_SPEED
 * Size is comprehensive of header, body and checksum
 * For NAK packets, selector is used to store the communication error
 */
typedef uint8_t header_t[4];

//! Type definition for a whole packet
typedef struct _packet_s {
  uint8_t header[4];
  uint8_t body[BODY_MAX_LEN+sizeof(crc_t)]; // Body + Checksum
} packet_t;


//! Packet types
typedef enum COM_TYPE_E {
  COM_TYPE_NULL,      // Reserved, never use
  COM_TYPE_HND,       // Handshake
  COM_TYPE_ACK,       // Acknowledgement
  COM_TYPE_NAK,       // Communication error
  COM_TYPE_ECHO,      // [DEBUG] Echo between Client and Master
  COM_TYPE_PING,      // Ping a slave controller
  COM_TYPE_GET_SPEED, // Get the current speed for a DC motor
  COM_TYPE_SET_SPEED, // Set (and apply) the speed for a DC motor
  COM_TYPE_APPLY,     // Tell all the slaves to apply the previously set speeds
  COM_TYPE_DAT,       // Primarily used for responses from the AVR device
  COM_TYPE_SET_SLAVE_ADDR, // Set new TWI address for a slave controller
  COM_TYPE_LIMIT      // Used for sanity checks - Must have highest value
} com_type_t;

#endif	// __PACKET_MODULE_COMMON_H
