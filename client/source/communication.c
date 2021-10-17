// AVR Multi Motor Control
/*! \file client/source/communication.c
 * Communication layer
 *
 * \author Paolo Lucchesi
 */
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "communication.h"
#include "serial.h"
#include "debug.h"
#include "packet.h"

#define ONE_MSEC 1000000 // One millisecond in nanoseconds


// Keep track of the current expected packet ID
static unsigned char packet_global_id;

// Compute the next packet global id
unsigned char packet_next_id(unsigned char current) {
  return (current + 1) % 0xFF;
}


// Estabilish a connection, sending a handshake (HND) packet
// Return 0 on success, 1 on failure
int communication_connect(int avr_fd) {
  packet_global_id = 0;
  return communication_craft_and_send(avr_fd, COM_TYPE_HND, 0, NULL, 0) ? 1 : 0;
}


// Attempt to receive a packet
// Return an appropriate error code (E_SUCCESS on success)
static unsigned char _recv_attempt(int avr_fd, packet_t *p) {
  unsigned char id, size=0;

  // Get and process packet header
  serial_rx(avr_fd, (void*) p, sizeof(header_t));
  id = packet_get_id(p);
  size = packet_get_size(p);

  if (id != packet_global_id) return E_ID_MISMATCH;

  // Get the remaining data
  serial_rx(avr_fd, ((void*) p) + sizeof(header_t), size - sizeof(header_t));

  return (packet_check_crc(p) != 0) ? E_CORRUPTED_CHECKSUM : E_SUCCESS;
}


// Send a packet
// Returns 0 on success, 1 on failure
// Never use for HND, ACK or ERR packet types
int communication_send(int avr_fd, const packet_t *p) {
  uint8_t size = packet_get_size(p);
  if (!avr_fd || !p || !size || size > sizeof(packet_t))
    return 1;

  debug {
    puts("\nSending packet");
    packet_print(p);
  }

  // Flush the serial RX buffers before beginning the communication
  serial_rx_flush(avr_fd);

  // Blindly send the packet on the serial port
  serial_tx(avr_fd, p, size);

  // Attempt to receive ACK/ERR
  // Assertion on ACK/ERR id is made inside the receive attempt function
  // If any packet different from an ACK one is received, take it as a failure
  packet_t response[1];
  uint8_t ret = _recv_attempt(avr_fd, response);
  debug err_log("_recv_attempt() returned %hhd", ret);

  debug {
    err_log((ret != E_SUCCESS) ? "Bad response received" :
        "Valid response received");
    packet_print(response);
  }

  if (ret == E_SUCCESS && packet_get_type(response) == COM_TYPE_ACK) {
    packet_global_id = packet_next_id(packet_global_id);
    debug err_log("Packet succesfully sent");
    return 0;
  }

  // An error occurred if landing here
  packet_global_id = 0;
  return 1;
}


// Receive a packet
// Returns 0 on success, 1 otherwise
// Never use for HND, ACK or ERR packet types
int communication_recv(int avr_fd, packet_t *p) {
  if (!avr_fd || !p) return 1;
  packet_t response[1];

  unsigned char ret = _recv_attempt(avr_fd, p);
  debug err_log("_recv_attempt() returned %hhd", ret);

  switch (ret) {
    case E_SUCCESS:
      packet_ack(p, response);
      serial_tx(avr_fd, response, PACKET_MIN_SIZE);
      packet_global_id = packet_next_id(packet_global_id);
      debug {
        err_log("Packet received successfully");
        packet_print(p);
      }
      return 0;

    case E_ID_MISMATCH:
    case E_CORRUPTED_CHECKSUM:
      packet_nak(p, response);
      serial_tx(avr_fd, response, PACKET_MIN_SIZE);
      debug err_log("Communication error: corrupted packet");
      return 1;

    default: return 1; // Can't land here in theory
  }
}


// Craft a packet in-place and send it
// Returns 0 on success, 1 on failure
// Never use for HND, ACK or ERR packet types
int communication_craft_and_send(int avr_fd, unsigned char type,
    unsigned char selector, const unsigned char *data,
    unsigned char data_size) {
  packet_t p[1];
  if (packet_craft(p, packet_global_id, type, selector, data, data_size) != 0)
    return 1;
  return communication_send(avr_fd, p);
}
