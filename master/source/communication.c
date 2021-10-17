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
#include "packet.h"
#include "crc.h"
#include "twi.h"

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


//! Communication handler state
typedef enum STATE_E {
  STATE_LISTEN,
  STATE_FETCH,
  STATE_EXECUTE,
  STATE_ACK,
  STATE_NAK
} state_t;

//! Callback for communication operation
typedef state_t (*operation_f)(const packet_t*);

// Do nothing on COM_TYPE_NULL
static state_t _op_null(const packet_t *p) {
  return STATE_NAK;
}

// Simply ACK a COM_TYPE_HND packet
static state_t _op_hnd(const packet_t *p) { return STATE_ACK; }

// DAT is not expected, do nothing
static state_t _op_dat(const packet_t *p) { return STATE_LISTEN; }

// When receiving an ACK, update the global packet id
static state_t _op_ack(const packet_t *p) {
  packet_global_id = packet_next_id(packet_global_id);
  return STATE_LISTEN;
}

// When receiving NAK, reset the global packet id
static state_t _op_nak(const packet_t *p) {
  packet_global_id = 0;
  return STATE_LISTEN;
}

// Echo back data to the host
static state_t _op_echo(const packet_t *p) {
  communication_send(COM_TYPE_ACK, 0, 0, NULL);
  packet_global_id = packet_next_id(packet_global_id);
  communication_send(COM_TYPE_DAT, 0,
      packet_get_size(p) - sizeof(header_t) - sizeof(crc_t), p->body);
  return STATE_LISTEN;
}


// Test the TWI module via single-character echoing
static state_t _op_twi_echo(const packet_t *p) {
  communication_send(COM_TYPE_ACK, 0, 0, NULL);
  packet_global_id = packet_next_id(packet_global_id);

  uint8_t tx[2] = { TWI_CMD_ECHO, p->body[0] };
  uint8_t data[4] = {0};

  //! \todo: Remove test 0x31 address
  data[1] = twi_send_sm(0x31, tx, 2);
  data[2] = twi_recv_sm(0x31, data, 1);
  data[3] = TWI_CMD_ECHO;
  communication_send(COM_TYPE_DAT, 0, sizeof(data), data);
  return STATE_LISTEN;
}

// Get the speed of the selected DC motors (0 for the others)
static state_t _op_get_speed(const packet_t *p) {
  //! \todo Sanity check on received packet
  communication_send(COM_TYPE_ACK, 0, 0, NULL);
  uint8_t motor_id = packet_get_selector(p) & DC_MOTOR_SEL_ALL;
  uint8_t speed = dcmotor_get(motor_id);
  communication_send(COM_TYPE_DAT, motor_id, sizeof(speed), &speed);
  return STATE_LISTEN;
}

// Get the speed of the selected DC motors (ignore the others)
static state_t _op_set_speed(const packet_t *p) {
  //! \todo Sanity check on received packet
  communication_send(COM_TYPE_ACK, 0, 0, NULL);
  uint8_t dc_motor_id = packet_get_selector(p) & DC_MOTOR_SEL_ALL;
  dc_rpm_t *data = (dc_rpm_t*) p->body;
  dcmotor_set(dc_motor_id, *data);
  return STATE_LISTEN;
}

// Tell a slave to apply the previously set speed
static state_t _op_apply(const packet_t *p) {
  dcmotor_apply();
  return STATE_ACK;
}


// Operation table
static operation_f op_table[] = {
  _op_null,
  _op_hnd,
  _op_ack,
  _op_nak,
  _op_echo,
  _op_twi_echo,
  _op_get_speed,
  _op_set_speed,
  _op_apply,
  _op_dat
};


// Communication handler routine
uint8_t communication_handler(void) {
  static state_t state = STATE_LISTEN;

  packet_t rx[1];
  uint8_t *_rx = (uint8_t*) rx;

  uint8_t received = 0;
  uint8_t to_recv;

  // Encountered error - In case of NAK, the selector field will be used
  // to communicate this
  uint8_t error = E_SUCCESS;

  // Main handler loop
  while (1) {
    uint8_t ret = 0;
    switch (state) {
      case STATE_LISTEN:  // Check for available data
        if (serial_rx_available()) ++state;
        else return ret;
        break;

      case STATE_FETCH:
        ret = 1; // At least a packet is processed

        // Receive packet header
        serial_rx(_rx, sizeof(header_t));
        received = sizeof(header_t);

        // Check on packet ID
        if (packet_get_id(rx) != packet_global_id) {
          error = E_ID_MISMATCH;
          state = STATE_NAK;
          break;
        }

        // Check on packet size
        if (packet_get_size(rx) >
            sizeof(header_t) + BODY_MAX_LEN + sizeof(crc_t)) {
          error = E_TOO_BIG;
          state = STATE_NAK;
          break;
        }

        // Receive packet body and checksum
        to_recv = packet_get_size(rx) - sizeof(header_t);
        serial_rx(_rx + received, to_recv);
        received += to_recv;

        // Check packet sanity
        if (crc_check(rx, packet_get_size(rx)) != 0) {
          error = E_CORRUPTED_CHECKSUM;
          state = STATE_NAK;
        }
        else ++state;
        break;

      case STATE_EXECUTE:  // React to the packet
        uint8_t packet_type = packet_get_type(rx);
        if (packet_type >= COM_TYPE_LIMIT) {
          error = E_WRONG_TYPE;
          state = STATE_NAK;
        }
        else state = op_table[packet_type](rx);
        break;

      case STATE_ACK:  // Acknowledge
        communication_send(COM_TYPE_ACK, 0, 0, NULL);
        packet_global_id = packet_next_id(packet_global_id);
        state = STATE_LISTEN;
        break;

      case STATE_NAK:  // Raise an error to the client
        communication_send(COM_TYPE_NAK, error, 0, NULL);
        packet_global_id = 0;
        state = STATE_LISTEN;
        break;
    }
  }
}
