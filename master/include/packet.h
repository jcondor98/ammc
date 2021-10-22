// AVR Multi Motor Control
/*! \file master/include/packet.h
 * Packet module (Master)
 *
 * \author Paolo Lucchesi
 */
#ifndef __PACKET_MODULE_H
#define __PACKET_MODULE_H
#include <stdint.h>
#include "crc.h"

#define BODY_MAX_LEN 32

/*!
 * Type definition for the packet header
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


//! Packet types
typedef enum COM_TYPE_E {
  //! Reserved, never use
  COM_TYPE_NULL,
  //! Handshake
  COM_TYPE_HND,
  //! Acknowledgement
  COM_TYPE_ACK,
  //! Communication error
  COM_TYPE_NAK,
  //! [DEBUG] Echo between Client and Master
  COM_TYPE_ECHO,
  //! [DEBUG] Echo a single char to the first Slave via TWI
  COM_TYPE_TWI_ECHO,
  //! Get the current speed for a DC motor
  COM_TYPE_GET_SPEED,
  //! Set (and apply) the speed for a DC motor
  COM_TYPE_SET_SPEED,
  //! Tell all the slaves to apply the previously set speeds
  COM_TYPE_APPLY,
  //! Primarily used for responses from the AVR device
  COM_TYPE_DAT,
  //! Set new TWI address for a slave controller
  COM_TYPE_SET_SLAVE_ADDR,
  //! Used for sanity checks - Must have highest value
  COM_TYPE_LIMIT
} com_type_t;


//! Various errors that can be encountered during packet communication
typedef enum _COM_ERROR_E {
  E_SUCCESS, E_ID_MISMATCH, E_CORRUPTED_CHECKSUM, E_WRONG_TYPE, E_TOO_BIG
} com_error_t;

//! Type definition for a whole packet
typedef struct _packet_s {
  header_t header;
  uint8_t body[BODY_MAX_LEN+sizeof(crc_t)]; // Body + Checksum
} packet_t;

//! Get the packet id
inline uint8_t packet_get_id(const packet_t *p)       { return p->header[0]; }

//! Get the packet type
inline uint8_t packet_get_type(const packet_t *p)     { return p->header[1]; }

//! Get the packet selector
inline uint8_t packet_get_selector(const packet_t *p) { return p->header[2]; }

//! Get the packet whole size
inline uint8_t packet_get_size(const packet_t *p)     { return p->header[3]; }

/*!
 * Craft a preallocated packet
 *
 * @param p Pointer to a preallocated packet data structure
 * @param id Packet id
 * @param type Packet type
 * @param selector Packet selector
 * @param body_size Size of 'body' in bytes
 * @param body Pointer to the area containing the body data
 * @returns 0 on success, 1 otherwise
 */
uint8_t packet_craft(packet_t *p, uint8_t id, uint8_t type, uint8_t selector,
    uint8_t body_size, const void *body);

#endif	// __PACKET_MODULE_H
