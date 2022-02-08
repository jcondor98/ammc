// AVR Multi Motor Control
/*! \file master/source/communication_ops.c
 * Communication module domain-specific operations
 *
 * \author Paolo Lucchesi
 */
#include "communication.h"
#include "master_commands.h"
#include "dcmotor.h"


// Echo back data to the host
static uint8_t _op_echo(const packet_t *p) {
  communication_send(COM_TYPE_DAT, 0, packet_get_body_size(p), p->body);
  return 0;
}

// Test the TWI module via single-character echoing
static uint8_t _op_ping(const packet_t *p) {
  const uint8_t slave_id = packet_get_selector(p);
  if (slave_id == 0 || slave_id > 127) {
    communication_send(COM_TYPE_DAT, E_SLAVE_NOT_FOUND, 0, NULL);
    return 0;
  }

  master_send_command(slave_id, CMD_PING, &slave_id, sizeof(slave_id));

  uint8_t response;
  uint8_t recvd = master_recv_response(slave_id, &response, sizeof(slave_id));
  uint8_t err_code = (recvd == sizeof(slave_id) && response == slave_id)
    ? E_SUCCESS : E_SLAVE_NOT_FOUND;
  communication_send(COM_TYPE_DAT, err_code, 0, NULL);

  return 0;
}

// Get the speed of the selected DC motors (0 for the others)
//! \todo Sanity check on received packet
static uint8_t _op_get_speed(const packet_t *p) {
  uint8_t motor_id = packet_get_selector(p) & DC_MOTOR_SEL_ALL;
  dc_rpm_t speed = dcmotor_get(motor_id);
  communication_send(COM_TYPE_DAT, motor_id, sizeof(speed), &speed);
  return 0;
}

// Get the speed of the selected DC motors (ignore the others)
//! \todo Sanity check on received packet
static uint8_t _op_set_speed(const packet_t *p) {
  uint8_t dc_motor_id = packet_get_selector(p) & DC_MOTOR_SEL_ALL;
  dc_rpm_t *data = (dc_rpm_t*) p->body;
  dcmotor_set(dc_motor_id, *data);
  return 0;
}

// Tell a slave to apply the previously set speed
static uint8_t _op_apply(const packet_t *p) {
  dcmotor_apply();
  return 0;
}

// Set a new TWI address for the selected slave
static uint8_t _op_set_slave_addr(const packet_t *p) {
  dcmotor_change_id(packet_get_selector(p), p->body[0]);
  return 0;
}


// Operation table
operation_f com_operation_table[] = {
  NULL, // NULL
  NULL, // HND
  NULL, // ACK
  NULL, // NAK
  _op_echo,
  _op_ping,
  _op_get_speed,
  _op_set_speed,
  _op_apply,
  NULL, // DAT
  _op_set_slave_addr
};
