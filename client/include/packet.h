// AVR Multi Motor Control
/*! \file client/include/packet.h
 * Packet module (Client)
 *
 * \author Paolo Lucchesi
 */
#ifndef __PACKET_MODULE_H
#define __PACKET_MODULE_H
#include <stdint.h>
#include "common/packet.h"

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
