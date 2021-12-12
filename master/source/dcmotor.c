// AVR Multi Motor Control
/*! \file master/source/dcmotor.c
 * DC Motor Master Module
 *
 * \author Paolo Lucchesi
 *
 * \todo Various integrity checks
 */
#include "dcmotor.h"
#include "master_commands.h"


// Get the speed of a DC motor in RPM
dc_rpm_t dcmotor_get(uint8_t slave_addr) {
  dc_rpm_t speed;
  master_send_command(slave_addr, CMD_GET_SPEED, NULL, 0);
  master_recv_response(slave_addr, &speed, sizeof(speed));
  return speed;
}

// Set the speed of a DC motor in RPM
void dcmotor_set(uint8_t slave_addr, dc_rpm_t speed) {
  master_send_command(slave_addr, CMD_SET_SPEED, &speed, sizeof(speed));
}

// Tell all the DC motor controllers to apply their speed
void dcmotor_apply(void) {
  master_send_bcast_command(CMD_APPLY_SPEED, NULL, 0);
}

// Set a new TWI address for the selected slave
void dcmotor_change_id(uint8_t actual_addr, uint8_t new_addr) {
  master_send_command(actual_addr, CMD_SET_ADDR, &new_addr, sizeof(new_addr));
}
