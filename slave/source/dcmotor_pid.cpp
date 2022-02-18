// AVR Multi Motor Control
/*! \file slave/sources/dcmotor_pid.c
 * DC Motor Proportional-Integral-Derivative Slave Module
 *
 * \author Paolo Lucchesi
 */
#include "dcmotor_pid.h"
#define ONE_SECOND_IN_MILLIS 1000

PidController::PidController(float kp, float ki, float kd, uint16_t interval_ms) {
  this->interval = interval_ms;
  this->kp = kp;
  this->ki = ki;
  this->kd = kd;
  this->err_int = 0;
  this->err_prev = 0;
}

float PidController::correct(float speed_actual, float speed_target) {
  float err_prop = speed_actual - speed_target;
  float err_der = (err_prop - err_prev)
    * ONE_SECOND_IN_MILLIS / interval;
  err_prev = err_prop;
  err_int += err_prop * interval / ONE_SECOND_IN_MILLIS;
  return kp * err_prop + ki * err_int + kd * err_der;
}
