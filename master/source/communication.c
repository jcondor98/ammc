// AVR Multi Motor Control
/*! \file master/source/communication.c
 * Communication module (server)
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>

#include "communication.h"
#include "dcmotor.h"
#include "serial.h"
#include "crc.h"
#include "twi.h"

//! Communication handler state
typedef enum STATE_E {
  STATE_RECEIVING, //!< Waiting for incoming packet
  STATE_HND,       //!< Handshake client
  STATE_ACK,       //!< Acknowledging packet
  STATE_NAK,       //!< Communication error
  STATE_EXECUTE,   //!< Executing the command issued by the client
} state_t;

//! Callback for communication operation
typedef state_t (*operation_f)(const packet_t*);


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

// Receive a packet
com_error_t communication_recv(packet_t *rx) {
  uint8_t *_rx = (uint8_t*) rx;
  uint8_t received = 0;
  uint8_t to_recv;
  uint8_t id, size, type; // Header parameters (just for convenience)

  // Receive packet header
  serial_rx(_rx, sizeof(header_t));
  received = sizeof(header_t);
  id = packet_get_id(rx);
  type = packet_get_type(rx);
  size = packet_get_size(rx);

  // Check on packet ID
  if (id != packet_global_id && (type != COM_TYPE_HND || id != 0))
    return E_ID_MISMATCH;

  // Check on packet size
  if (size > sizeof(header_t) + BODY_MAX_LEN + sizeof(crc_t))
    return E_TOO_BIG;

  // Receive packet body and checksum
  to_recv = size - sizeof(header_t);
  serial_rx(_rx + received, to_recv);
  received += to_recv;

  // Check packet type and checksum
  if (type >= COM_TYPE_LIMIT)   return E_WRONG_TYPE;
  if (crc_check(rx, size) != 0) return E_CORRUPTED_CHECKSUM;

  return E_SUCCESS;
}


// Echo back data to the host
static state_t _op_echo(const packet_t *p) {
  communication_send(COM_TYPE_DAT, 0,
      packet_get_size(p) - sizeof(header_t) - sizeof(crc_t), p->body);
  return STATE_RECEIVING;
}

// Test the TWI module via single-character echoing
static state_t _op_twi_echo(const packet_t *p) {
  uint8_t tx[2] = { TWI_CMD_ECHO, p->body[0] };
  uint8_t data[4] = {0};
  //! \todo: Remove test 0x31 address
  data[1] = twi_send_sm(0x31, tx, 2);
  data[2] = twi_recv_sm(0x31, data, 1);
  data[3] = TWI_CMD_ECHO;
  communication_send(COM_TYPE_DAT, 0, sizeof(data), data);
  return STATE_RECEIVING;
}

// Get the speed of the selected DC motors (0 for the others)
static state_t _op_get_speed(const packet_t *p) {
  //! \todo Sanity check on received packet
  uint8_t motor_id = packet_get_selector(p) & DC_MOTOR_SEL_ALL;
  dc_rpm_t speed = dcmotor_get(motor_id);
  communication_send(COM_TYPE_DAT, motor_id, sizeof(speed), &speed);
  return STATE_RECEIVING;
}

// Get the speed of the selected DC motors (ignore the others)
static state_t _op_set_speed(const packet_t *p) {
  //! \todo Sanity check on received packet
  uint8_t dc_motor_id = packet_get_selector(p) & DC_MOTOR_SEL_ALL;
  dc_rpm_t *data = (dc_rpm_t*) p->body;
  dcmotor_set(dc_motor_id, *data);
  return STATE_RECEIVING;
}

// Tell a slave to apply the previously set speed
static state_t _op_apply(const packet_t *p) {
  dcmotor_apply();
  return STATE_RECEIVING;
}

// Set a new TWI address for the selected slave
static state_t _op_set_slave_addr(const packet_t *p) {
  dcmotor_change_id(packet_get_selector(p), p->body[0]);
  return STATE_RECEIVING;
}

// Operation table
static operation_f op_table[] = {
  NULL, // NULL
  NULL, // HND
  NULL, // ACK
  NULL, // NAK
  _op_echo,
  _op_twi_echo,
  _op_get_speed,
  _op_set_speed,
  _op_apply,
  NULL, // DAT
  _op_set_slave_addr
};


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
        operation_f action = op_table[packet_get_type(rx)];
        state = action ? action(rx) : STATE_RECEIVING;
        break;
    }
  }
}
