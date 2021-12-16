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
#include "dcmotor_phy.h"

//! Initialize DC motor handling
void dcmotor_init(void);

//! @return The last sampled speed (internally stored)
dc_rpm_t dcmotor_get(void);

//! Set a new speed (without applying it)
void dcmotor_set(dc_rpm_t);

//! Apply the previously given RPM value
void dcmotor_apply(void);

//! Start the PID controller activity
#define dcmotor_start_pid() dcmotor_phy_start_pid()

//! Stop the PID controller activity
#define dcmotor_stop_pid() dcmotor_phy_stop_pid()

#endif	// __DC_MOTOR_MODULE_H
