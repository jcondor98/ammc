// AVR Multi Motor Control
/*! \file slave/include/dcmotor_pid.h
 * DC Motor types and declarations
 *
 * \author Paolo Lucchesi
 */
#ifndef __DCMOTOR_TYPES_INTERFACE_H
#define __DCMOTOR_TYPES_INTERFACE_H

#ifndef DC_MOTOR_MAX_RPM
//! Max RPM for a DC motor
#define DC_MOTOR_MAX_RPM 200
#endif

#ifndef DC_ENC_SIGNALS_PER_ROUND
//! Number of signals generated by the encoder for each round
#define DC_ENC_SIGNALS_PER_ROUND 8
#endif

//! Real RPM speed sampling interval in milliseconds
#define DC_SAMPLING_INTERVAL 100

//! Type to properly store Rounds Per Minutes
typedef uint16_t dc_rpm_t;

#endif	// __DCMOTOR_TYPES_INTERFACE_H
