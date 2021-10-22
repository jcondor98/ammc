// AVR Multi Motor Control
/*! \file client/include/packet.h
 * Packet module (Client)
 *
 * \author Paolo Lucchesi
 */
#ifndef __PACKET_MODULE_H
#define __PACKET_MODULE_H
#include <stdint.h>
#include "crc.h"

#define BODY_MAX_LEN 32
#define PACKET_MIN_SIZE (sizeof(header_t) + sizeof(crc_t))

/*!
 * Type definition for packet headers
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
  //! [DEBUG] Echo between Master and Slave via TWI
  COM_TYPE_ECHO_TWI,
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

//! Type definition for a whole packet
typedef struct _packet_s {
  uint8_t header[4];
  uint8_t body[BODY_MAX_LEN+sizeof(crc_t)]; // Body + Checksum
} packet_t;

/*!
 * Getter for header ID field
 *
 * @param p Pointer to the packet
 * @returns The packet ID
 */
inline uint8_t packet_get_id(const packet_t *p) { return p->header[0]; }

/*!
 * Getter for header type field
 *
 * @param p Pointer to the packet
 * @returns The packet type
 */
inline uint8_t packet_get_type(const packet_t *p) { return p->header[1]; }

/*!
 * Getter for header selector field
 *
 * @param p Pointer to the packet
 * @returns The packet selector
 */
inline uint8_t packet_get_selector(const packet_t *p) { return p->header[2]; }

/*!
 * Getter for header size field
 *
 * @param p Pointer to the packet
 * @returns The whole packet size, counting header and CRC
 */
inline uint8_t packet_get_size(const packet_t *p) { return p->header[3]; }

/*!
 * Craft a preallocated packet
 *
 * @param p Pointer to the buffer to store the crafted packet
 * @param id        Packet ID
 * @param type      Packet type
 * @param selector  Value for the selector field
 * @param body      Pointer to the data to fill the packet body
 * @param body_size Size of 'body' in bytes
 * @returns 0 on success, 1 on failure
 */
uint8_t packet_craft(packet_t *p, uint8_t id, uint8_t type, uint8_t selector,
    const void *body, uint8_t body_size);

/*!
 * Check packet data integrity via CRC
 *
 * @param p Pointer to the packet to check
 * @returns 0 if the packet is sane, 1 otherwise
 */
uint8_t packet_check_crc(const packet_t *p);

/*!
 * Given an existing packet, craft an ACK packet
 *
 * @param p Pointer to the previously received packet to ACK
 * @param response Pointer to the buffer to store the ACK response
 * @returns 0 on success, 1 otherwise
 */
uint8_t packet_ack(const packet_t *p, packet_t *response);

/*!
 * Given an existing packet, craft a NAK packet
 *
 * @param p Pointer to the previously received packet to NAK
 * @param response Pointer to the buffer to store the NAK response
 * @returns 0 on success, 1 otherwise
 */
uint8_t packet_nak(const packet_t *p, packet_t *response);

#ifdef DEBUG
/*!
 * Dump a packet
 *
 * @param p Pointer to the packet to dump
 */
void packet_print(const packet_t *p);
#endif

#endif	// __PACKET_MODULE_H
