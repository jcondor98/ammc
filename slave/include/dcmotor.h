// AVR Multi Motor Control
/*! \file slave/include/dcmotor.h
 * DC Motor Slave Module
 *
 * \author Paolo Lucchesi
 */
#ifndef __DC_MOTOR_MODULE_H
#define __DC_MOTOR_MODULE_H
#include <stdint.h>

//! Max RPM for a DC motor
#define DC_MOTOR_MAX_RPM 200

//! Number of signals generated by the encoder for each round
#define DC_ENC_SIGNALS_PER_ROUND 8

//! Real RPM speed sampling interval in milliseconds
#define DC_SAMPLING_INTERVAL 100

//! Type to properly store Rounds Per Minutes
typedef uint8_t dc_rpm_t;

//! Master -> Slave commands
typedef enum _DC_MOTOR_CMD_E {
  DC_MOTOR_CMD_GET,
  DC_MOTOR_CMD_SET,
  DC_MOTOR_CMD_APPLY,
  TWI_CMD_ECHO
} dc_motor_cmd_t;

//! Initialize DC motor handling
void dcmotor_init(void);

//! @return The last sampled speed (internally stored)
dc_rpm_t dcmotor_get(void);

//! Set a new speed (without applying it)
void dcmotor_set(dc_rpm_t);

//! Apply the previously given RPM value
void dcmotor_apply(void);

//! Start the DC motor PID sampling timer
void dcmotor_pid_start(void);

//! Stop the DC motor PID sampling timer
void dcmotor_pid_stop(void);

#endif	// __DC_MOTOR_MODULE_H
