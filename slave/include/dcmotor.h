// AVR Multi Motor Control
/*! \file slave/include/dcmotor.h
 * DC Motor Slave Module
 *
 * \author Paolo Lucchesi
 */
#ifndef __DC_MOTOR_MODULE_H
#define __DC_MOTOR_MODULE_H
#include <stdint.h>
#include "common/dcmotor.h"

//! Initialize DC motor handling
void dcmotor_init(void);

//! @return The last sampled speed (internally stored)
dc_rpm_t dcmotor_get(void);

//! Set a new speed (without applying it)
void dcmotor_set(dc_rpm_t);

//! Apply the previously given RPM value
void dcmotor_apply(void);

#endif	// __DC_MOTOR_MODULE_H
