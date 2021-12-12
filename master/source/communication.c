// AVR Multi Motor Control
/*! \file master/source/communication.c
 * Communication module (server)
 *
 * \author Paolo Lucchesi
 * \todo Move the operations outside of the communication module
 */
#include <avr/io.h>
#include <stddef.h>

#include "communication.h"
#include "serial.h"
#include "crc.h"


// Operation table
// TODO: Make a function to set this instead of importing like this
extern operation_f com_operation_table[];

// Global ID for packets
// In case of NAK, this variable is reset to 0
static uint8_t packet_global_id = 0;

// Compute the next packet global ID
static uint8_t packet_next_id(uint8_t current) {
  return (current + 1) % 0xFF;
}

// Count the number of set bits in a chunk of data
static inline uint8_t _count_bits(const void *data, size_t size) {
  static uint8_t lookup[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
  const uint8_t *_data = data;
  uint8_t count = 0;
  for (size_t i=0; i < size; ++i)
    count += lookup[_data[i] & 0x0F] + lookup[_data[i] >> 0x0F];
  return count;
}

// Create in-place and send a packet
void communication_send(uint8_t type, uint8_t selector,
    uint8_t body_size, const void *body) {
  packet_t p[1];
  packet_craft(p, packet_global_id, type, selector, body_size, body);
  serial_tx((uint8_t*) p, packet_get_size(p));
}

// Check packet header sanity
static inline com_error_t packet_header_sanity(const packet_t *p) {
  uint8_t id = packet_get_id(p);
  uint8_t type = packet_get_type(p);
  uint8_t size = packet_get_size(p);
  // Check on packet ID
  if (id != packet_global_id && (type != COM_TYPE_HND || id != 0))
    return E_ID_MISMATCH;
  // Check on packet size
  if (size > sizeof(header_t) + BODY_MAX_LEN + sizeof(crc_t))
    return E_TOO_BIG;
  return E_SUCCESS; // Sane packet header
}

// Check packet sanity (not considering header)
static inline com_error_t packet_sanity(const packet_t *p) {
  if (packet_get_type(p) >= COM_TYPE_LIMIT)  return E_WRONG_TYPE;
  if (crc_check(p, packet_get_size(p)) != 0) return E_CORRUPTED_CHECKSUM;
  return E_SUCCESS; // Sane packet
}

static inline void recv_packet_header(packet_t *rx) {
  serial_rx((void*) rx, sizeof(header_t));
}

static inline void recv_packet_body(packet_t *rx) {
  void *rx_body = ((void*) rx) + sizeof(header_t);
  uint8_t body_size = packet_get_size(rx) - sizeof(header_t);
  serial_rx(rx_body, body_size);
}

com_error_t communication_recv(packet_t *rx) {
  recv_packet_header(rx);
  com_error_t header_sanity = packet_header_sanity(rx);
  if (header_sanity != E_SUCCESS)
    return header_sanity;
  recv_packet_body(rx);
  return packet_sanity(rx);
}


//! Communication handler state
typedef enum STATE_E {
  STATE_RECEIVING, //!< Waiting for incoming packet
  STATE_HND,       //!< Handshake client
  STATE_ACK,       //!< Acknowledging packet
  STATE_NAK,       //!< Communication error
  STATE_EXECUTE,   //!< Executing the command issued by the client
} state_t;


// Communication handler routine
uint8_t communication_handler(void) {
  static state_t state = STATE_RECEIVING;

  packet_t rx[1];
  uint8_t error = E_SUCCESS;
  uint8_t ret = 0;

  // Main handler loop
  while (1) {
    switch (state) {
      case STATE_RECEIVING: // Check for available data
        if (!serial_rx_available()) return ret;

        ret = 1; // At least a packet is processed
        error = communication_recv(rx);

        if (error != E_SUCCESS)
          state = STATE_NAK;
        else if (packet_get_type(rx) == COM_TYPE_HND)
          state = STATE_HND;
        else state = STATE_ACK;
        break;

      case STATE_HND: // Handshake
        packet_global_id = 0;
        state = STATE_ACK;
        break;

      case STATE_ACK: // Acknowledge
        communication_send(COM_TYPE_ACK, 0, 0, NULL);
        packet_global_id = packet_next_id(packet_global_id);
        state = STATE_EXECUTE;
        break;

      case STATE_NAK: // Raise an error to the client
        communication_send(COM_TYPE_NAK, error, 0, NULL);
        packet_global_id = 0;
        state = STATE_RECEIVING;
        break;

      case STATE_EXECUTE: // React to the packet
        operation_f action = com_operation_table[packet_get_type(rx)];
        state = action ? action(rx) : STATE_RECEIVING;
        break;
    }
  }
}
