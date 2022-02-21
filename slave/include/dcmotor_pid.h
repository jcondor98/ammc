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
   * @param samplingInterval The dc motor sampling interval (milliseconds)
   */
  constexpr PidController(
      float kp,
      float ki,
      float kd,
      uint16_t samplingInterval
  );

  /*!
   * Perform a PID controller iteration
   *
   * @param actualSpeed The actual speed of the dc motor
   * @param targetSpeed The target speed of the dc motor
   * @returns The PID-corrected dc motor target speed
   */
  float correct(float actualSpeed, float targetSpeed);

 private:
  float kp, ki, kd;
  float intError, prevError;
  uint16_t interval;
};
