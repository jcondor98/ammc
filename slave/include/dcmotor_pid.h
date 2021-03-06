// AVR Multi Motor Control
/*! \file slave/include/dcmotor_pid.h
 * DC Motor Proportional-Integral-Derivative Slave Module
 *
 * \author Paolo Lucchesi
 */
#ifndef __DCMOTOR_PID_MODULE_H
#define __DCMOTOR_PID_MODULE_H
#include "common/dcmotor.h"

/*!
 * Initialize the PID controller
 *
 * @param kp The proportional weight constant
 * @param ki The integral weight constant
 * @param kd The derivative weight constant
 * @param sample_int The dc motor sampling interval
 */
void dcmotor_pid_init(float kp, float ki, float kd, uint16_t sample_int);

/*!
 * Perform a PID controller iteration
 *
 * @param speed_actual The actual speed of the dc motor
 * @param speed_target The target speed of the dc motor
 * @returns The PID-corrected dc motor target speed
 */
float dcmotor_pid_iterate(float speed_actual, float speed_target);

#endif	// __DCMOTOR_PID_MODULE_H
