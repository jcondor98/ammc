// AVR Multi Motor Control
/*! \file slave/sources/dcmotor_pid.c
 * DC Motor Proportional-Integral-Derivative Slave Module
 *
 * \author Paolo Lucchesi
 */
#include "dcmotor_pid.h"
#define ONE_SECOND_IN_MILLIS 1000

constexpr PidController::PidController(
    float kp,
    float ki,
    float kd,
    uint16_t interval
) : kp(kp),
    ki(ki),
    kd(kd),
    intError(0),
    prevError(0),
    interval(interval) {}

float PidController::correct(float actualSpeed, float targetSpeed) {
  float propError = actualSpeed - targetSpeed;
  float derError = (propError - prevError)
    * ONE_SECOND_IN_MILLIS / interval;
  prevError = propError;
  intError += propError * interval / ONE_SECOND_IN_MILLIS;
  return kp * propError + ki * intError + kd * derError;
}
