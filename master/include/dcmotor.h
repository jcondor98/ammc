// AVR Multi Motor Control
/*! \file master/include/dcmotor.h
 * DC Motor Master Module
 *
 * \author Paolo Lucchesi
 * \todo Make a common header file?
 */
#ifndef __DC_MOTOR_MODULE_H
#define __DC_MOTOR_MODULE_H
#include <stdint.h>
#include "common/dcmotor.h"

/*!
 * Get the speed of a DC motor in RPM
 *
 * @param slave_addr Slave address
 * @return The speed of the DC motor handled by the addressed I2C slave
 */
dc_rpm_t dcmotor_get(uint8_t slave_addr);

/*!
 * Set the speed of a DC motor in RPM
 *
 * @param slave_addr Slave address
 * @param rpm Next target DC motor speed in RPM
 */
void dcmotor_set(uint8_t slave_addr, dc_rpm_t speed);

//! Tell all the DC motor controllers to apply their speed
void dcmotor_apply(void);

/*!
 * Set a new TWI address for the selected slave
 *
 * @param actual_addr Current slave address
 * @param new_addr New slave address to set
 */
void dcmotor_change_id(uint8_t actual_addr, uint8_t new_addr);

#endif	// __DC_MOTOR_MODULE_H
