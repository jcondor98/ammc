// AVR Multi Motor Control
/*! \file master/include/packet.h
 * Packet module (Master)
 *
 * \author Paolo Lucchesi
 */
#ifndef __PACKET_MODULE_H
#define __PACKET_MODULE_H
#include <stdint.h>
#include "common/packet.h"
#include "crc.h"

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
 * @param p Preallocated packet data structure
 * @param id Packet id
 * @param type Packet type
 * @param selector Packet selector
 * @param body_size Size of 'body' in bytes
 * @param body Body data
 * @returns 0 on success, 1 otherwise
 */
uint8_t packet_craft(packet_t *p, uint8_t id, uint8_t type, uint8_t selector,
    uint8_t body_size, const void *body);

#endif	// __PACKET_MODULE_H
