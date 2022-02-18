// AVR Multi Motor Control
/*! \file slave/include/dcmotor_pid.h
 * DC Motor Proportional-Integral-Derivative Slave Module
 *
 * \author Paolo Lucchesi
 */
#pragma once
#include <stdint.h>
#include "common/dcmotor.h"

class PidController {
 public:
  /*!
   * @param kp The proportional weight constant
   * @param ki The integral weight constant
   * @param kd The derivative weight constant
   * @param sample_int The dc motor sampling interval
   */
  PidController(float kp, float ki, float kd, uint16_t sample_int);

  /*!
   * Perform a PID controller iteration
   *
   * @param speed_actual The actual speed of the dc motor
   * @param speed_target The target speed of the dc motor
   * @returns The PID-corrected dc motor target speed
   */
  float correct(float speed_actual, float speed_target);

 private:
  float kp, ki, kd;
  float err_int, err_prev;
  uint16_t interval;
};
