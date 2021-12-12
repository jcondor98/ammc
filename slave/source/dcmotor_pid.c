// AVR Multi Motor Control
/*! \file slave/sources/dcmotor_pid.c
 * DC Motor Proportional-Integral-Derivative Slave Module
 *
 * \author Paolo Lucchesi
 */
#include "dcmotor_pid.h"

#define ONE_SECOND 60000


static uint8_t sampling_interval;
static float k_prop, k_int, k_der;
static float err_int, err_prev;


void dcmotor_pid_init(float kp, float ki, float kd, uint8_t sample_int) {
  k_prop = kp;
  k_int = ki;
  k_der = kd;
  sampling_interval = sample_int;
  err_int = 0;
  err_prev = 0;
}


dc_rpm_t dcmotor_pid_iterate(dc_rpm_t speed_actual, dc_rpm_t speed_target) {
  float err_prop = speed_actual - speed_target;
  err_int += err_prop * sampling_interval / ONE_SECOND;
  float err_der = (err_prop - err_prev) * ONE_SECOND / sampling_interval;

  err_prev = err_prop;

  return k_prop * err_prop + k_int * err_int + k_der * err_der;
}
