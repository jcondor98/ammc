// AVR Multi Motor Control
/*! \file client/source/packet.c
 * Packet module
 *
 * \author Paolo Lucchesi
 */
#include <stdint.h>
#include <string.h>

#include "packet.h"

#ifdef DEBUG
#include <stdio.h>
#include "debug.h"
#endif


// Compose packet header
static void packet_header(packet_t *p, uint8_t id, uint8_t type,
    uint8_t selector, uint8_t size) {
  p->header[0] = id;
  p->header[1] = type;
  p->header[2] = selector;
  p->header[3] = size;
}


// Craft a preallocated packet
uint8_t packet_craft(packet_t *p, uint8_t id, uint8_t type, uint8_t selector,
    const void *body, uint8_t body_size) {
  if (!p || body_size > BODY_MAX_LEN || (!body && body_size) ||
      type >= COM_TYPE_LIMIT)
    return 1;

  // Compose packet header
  uint8_t size = sizeof(header_t) + body_size + sizeof(crc_t);
  packet_header(p, id, type, selector, size);

  // Compose packet body
  if (body) memcpy(p->body, body, body_size);

  // Compute and attach checksum
  crc_t cksum = crc(p, packet_get_size(p) - sizeof(cksum));
  memcpy(p->body + body_size, &cksum, sizeof(cksum));

  return 0;
}


// Check packet data integrity via CRC
// Returns 0 if the packet is sane, 1 otherwise
uint8_t packet_check_crc(const packet_t *p) {
  return crc_check((const void*) p, packet_get_size(p)) ? 1 : 0;
}


// Given an existing packet, craft an ACK packet
// Returns 0 on success, 1 otherwise
uint8_t packet_ack(const packet_t *p, packet_t *response) {
  if (!p || !response) return 1;
  return packet_craft(response, packet_get_id(p), COM_TYPE_ACK, 0, NULL, 0);
}


// Given an existing packet, craft a NAK packet
// Returns 0 on success, 1 otherwise
uint8_t packet_nak(const packet_t *p, packet_t *response) {
  if (!p || !response) return 1;
  return packet_craft(response, packet_get_id(p), COM_TYPE_NAK, 0, NULL, 0);
}

#ifdef DEBUG
// [DEBUG] Dump a packet
void packet_print(const packet_t *p) {
  if (!p) {
    printf("Pointer to packet is NULL\n");
    return;
  }

  printf("sizeof(header_t): %zu\n", sizeof(header_t));

  const uint8_t id   = packet_get_id(p);
  const uint8_t type = packet_get_type(p);
  const uint8_t sel  = packet_get_selector(p);
  const uint8_t size = packet_get_size(p);

  static const char type_str[COM_TYPE_LIMIT][24] = {
    "NULL", "HND", "ACK", "NAK", "ECHO", "PING", "GET_SPEED", "SET_SPEED",
    "APPLY", "DAT", "SET_SLAVE_ADDR"
  };

  printf("\nPrinting packet\n"
         "ID  : %d\n"
         "Type: %s\n"
         "Sel : %hhx\n"
         "Size: %d\n"
         "CRC : %hhx\n",
      id, type >= COM_TYPE_LIMIT ? "UNKNOWN" : type_str[type], sel, size,
      ((uint8_t*) p)[size - 1]);

  printf("Raw Data (hex):");
  for (uint8_t i=0; i < size; ++i)
    printf(" %2hhx", ((uint8_t*) p)[i]);
  putchar('\n');
  putchar('\n');
}
#endif
