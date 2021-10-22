// AVR Multi Motor Control
/*! \file master/source/dcmotor.c
 * DC Motor Master Module
 *
 * \author Paolo Lucchesi
 *
 * \todo Various integrity checks
 */
#include <util/delay.h>
#include <string.h>
#include "dcmotor.h"
#include "twi.h"


// Get the speed of a DC motor in RPM
dc_rpm_t dcmotor_get(uint8_t slave_addr) {
  uint8_t cmd = DC_MOTOR_CMD_GET;
  dc_rpm_t data_rx[1];

  // Fetch the speed from the slave
  twi_send_sm(slave_addr, &cmd, 1);
  twi_recv_sm(slave_addr, (void*) data_rx, sizeof(dc_rpm_t));

  return *data_rx;
}


// Set the speed of a DC motor in RPM
uint8_t dcmotor_set(uint8_t slave_addr, dc_rpm_t rpm) {
  uint8_t data[1 + sizeof(dc_rpm_t)];
  data[0] = DC_MOTOR_CMD_SET; // Master -> Slave command
  memcpy(data + 1, &rpm, sizeof(dc_rpm_t)); // RPM value

  // Send command to indicated slave
  twi_send_sm(slave_addr, data, 1 + sizeof(dc_rpm_t));

  return 0;
}


// Tell all the DC motor controllers to apply their speed
void dcmotor_apply(void) {
  uint8_t cmd = DC_MOTOR_CMD_APPLY;
  twi_bcast_sm(&cmd, 1);
}


// Set a new TWI address for the selected slave
void dcmotor_change_id(uint8_t actual_addr, uint8_t new_addr) {
  uint8_t data[] = { TWI_CMD_SET_ADDR, new_addr };
  twi_send_sm(actual_addr, data, sizeof(data));
}
