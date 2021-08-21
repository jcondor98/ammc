// AVR Multi Motor Control -- Paolo Lucchesi
// Packet module (Master) - Head file
#include <string.h>
#include "packet.h"

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
    uint8_t body_size, const void *body) {
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
