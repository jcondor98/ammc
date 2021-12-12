// AVR Multi Motor Control
/*! \file master/include/dcmotor.h
 * DC Motor Master Module
 *
 * \author Paolo Lucchesi
 */
#ifndef __DC_MOTOR_MODULE_H
#define __DC_MOTOR_MODULE_H
#include <stdint.h>

//! \todo Set this dynamically, also using the EEPROM
//! Maximum speed of a motor in RPM
#define DC_MOTOR_MAX_SPEED 3100

//! Handle DC motor number
#define DC_MOTOR_NUM 4

//! Predefined selector value with all motors selected
#define DC_MOTOR_SEL_ALL (0xFF << (8-DC_MOTOR_NUM) >> (8-DC_MOTOR_NUM))

//! Type to properly store a rounds per minute value
typedef int16_t dc_rpm_t;

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
