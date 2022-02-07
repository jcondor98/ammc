// AVR Multi Motor Control
/*! \file slave/sources/dcmotor_pid.c
 * DC Motor Proportional-Integral-Derivative Slave Module
 *
 * \author Paolo Lucchesi
 */
#include "dcmotor_phy.h"
#include "dcmotor_pid.h"

#define ONE_SECOND 60000.0

static uint16_t pid_interval;
static float k_prop, k_int, k_der;
float err_int, err_prev;

void dcmotor_pid_init(float kp, float ki, float kd, uint16_t pid_interval_ms) {
  pid_interval = pid_interval_ms;
  k_prop = kp;
  k_int = ki;
  k_der = kd;
  err_int = 0;
  err_prev = 0;
}

dc_rpm_t dcmotor_pid_iterate(dc_rpm_t speed_actual, dc_rpm_t speed_target) {
  float err_prop = speed_actual - speed_target;
  err_int += err_prop * pid_interval / ONE_SECOND;
  float err_der = (err_prop - err_prev) * ONE_SECOND / pid_interval;

  err_prev = err_prop;

  return speed_actual - (k_prop * err_prop + k_int * err_int + k_der * err_der);
}
