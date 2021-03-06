// AVR Multi Motor Control
/*! \file slave/source/dcmotor.c
 * DC Motor Slave Module
 * \todo Use 16-bit PWM
 *
 * \author Paolo Lucchesi
 */
#include <avr/interrupt.h>
#include "common/dcmotor.h"
#include "dcmotor_phy.h"
#include "dcmotor_phy_params.h"
#include "dcmotor.h"
#include "dcmotor_pid.h"

#define DCMOTOR_PID_KP 1.5
#define DCMOTOR_PID_KI 3.7
#define DCMOTOR_PID_KD 0.0

#define max(x,y) ((x) > (y) ? (x) : (y))
#define min(x,y) ((x) < (y) ? (x) : (y))


static volatile int32_t motor_position;
static volatile dc_rpm_t speed_actual;
static dc_rpm_t speed_target;
static dc_rpm_t speed_next;


void dcmotor_init(void) {
  dcmotor_pid_init(
      DCMOTOR_PID_KP,
      DCMOTOR_PID_KI,
      DCMOTOR_PID_KD,
      DC_SAMPLING_INTERVAL
    );
  dcmotor_phy_pid_init(DC_SAMPLING_INTERVAL);
  dcmotor_phy_encoder_init();
  dcmotor_phy_pwm_init();
  dcmotor_phy_load_speed(0);
}

dc_rpm_t dcmotor_get(void) {
  return speed_actual;
}

static inline dc_rpm_t compute_limited_speed(dc_rpm_t speed) {
  return (speed >= 0)
    ? min(speed, DC_MOTOR_MAX_RPM_SPEED)
    : max(speed, DC_MOTOR_MAX_RPM_SPEED);
}

void dcmotor_set(dc_rpm_t next) {
  speed_next = compute_limited_speed(next);
}

void dcmotor_apply(void) {
  speed_target = speed_next;
}


// Encoder ISR
ISR(DCMOTOR_PHY_ENCODER_ISR) {
  uint8_t enc_a = dcmotor_phy_read_encoder_phase_a();
  uint8_t enc_b = dcmotor_phy_read_encoder_phase_b();
  if (enc_a) motor_position += enc_b ? 1 : -1;
}


// TODO: Use float variables and constants?
static inline float compute_speed_from_position(int32_t _position) {
  double position = (double) _position;
  return position / DC_ENC_SIGNALS_PER_ROUND
    * 60000 / DC_SAMPLING_INTERVAL;
}

// PID sampling timer ISR
ISR(DCMOTOR_PHY_PID_ISR) {
  float speed = compute_speed_from_position(motor_position);
  float corrected_speed = dcmotor_pid_iterate(speed, speed_target);

  dcmotor_phy_load_speed_float(corrected_speed);
  speed_actual = (dc_rpm_t) speed;
  motor_position = 0;
}
