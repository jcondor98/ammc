// AVR Multi Motor Control
/*! \file slave/source/dcmotor_phy.c
 * DC Motor Slave Hardware Abstraction Layer
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>
#include "common/dcmotor.h"
#include "dcmotor_phy.h"
#include "dcmotor_phy_params.h"

#define OCR_ONE_MSEC 15.625
#define abs(x) ((x) > 0 ? (x) : -(x))
#define min(x,y) ((x) < (y) ? (x) : (y))

typedef uint8_t duty_cycle_t;

typedef enum _DIRECTION_E {
  DIR_CLOCKWISE, DIR_COUNTER_CLOCKWISE
} direction_t;


static inline void set_direction(direction_t dir) {
  if (dir == DIR_CLOCKWISE) {
    DIRECTION_IN1_PORT |=  DIRECTION_IN1_PORT_MASK;
    DIRECTION_IN2_PORT &= ~DIRECTION_IN2_PORT_MASK;
  }
  else if (dir == DIR_COUNTER_CLOCKWISE) {
    DIRECTION_IN1_PORT &= ~DIRECTION_IN1_PORT_MASK;
    DIRECTION_IN2_PORT |=  DIRECTION_IN2_PORT_MASK;
  }
}

static inline duty_cycle_t rpm2pwm(dc_rpm_t rpm) {
  static const double max_speed = DC_MOTOR_MAX_RPM_SPEED;
  static const double pwm_upper_bound = 0xFF;
  const double speed = rpm;
  return (duty_cycle_t) (speed / max_speed * pwm_upper_bound);
}

static inline dc_rpm_t bound_speed(dc_rpm_t rpm) {
  return min(rpm, DC_MOTOR_MAX_RPM_SPEED);
}

static inline void load_speed_clockwise(duty_cycle_t duty_cycle) {
  set_direction(DIR_CLOCKWISE);
  SPEED_REGISTER = duty_cycle;
}

static inline void load_speed_counter_clockwise(duty_cycle_t duty_cycle) {
  set_direction(DIR_COUNTER_CLOCKWISE);
  SPEED_REGISTER = duty_cycle;
}

void dcmotor_phy_load_speed(dc_rpm_t speed) {
  duty_cycle_t duty_cycle = rpm2pwm(bound_speed(abs(speed)));
  if (speed >= 0) load_speed_clockwise(duty_cycle);
  else load_speed_counter_clockwise(duty_cycle);
}

void dcmotor_phy_load_speed_float(float speed) {
  dcmotor_phy_load_speed((dc_rpm_t) speed);
}

void dcmotor_phy_pwm_init(void) {
  PWM_DDR |= PWM_DDR_MASK;
  PWM_TCCRA = PWM_TCCRA_VALUE;
  PWM_TCCRB = PWM_TCCRB_VALUE;
  DIRECTION_IN1_DDR |=  DIRECTION_IN1_DDR_MASK;
  DIRECTION_IN2_DDR |=  DIRECTION_IN2_DDR_MASK;
}

void dcmotor_phy_encoder_init(void) {
  ENCODER_A_DDR  &= ~ENCODER_A_DDR_MASK;
  ENCODER_B_DDR  &= ~ENCODER_B_DDR_MASK;
  ENCODER_A_PORT |= ENCODER_A_PORT_MASK;
  ENCODER_B_PORT |= ENCODER_B_PORT_MASK;
  ENCODER_PCMSK  |= ENCODER_PCMSK_MASK;
  ENCODER_PCICR  |= ENCODER_PCICR_MASK;
}

//! @todo Do not use conditionals
uint8_t dcmotor_phy_read_encoder_phase_a(void) {
  return (ENCODER_A_PIN & ENCODER_A_PIN_MASK) ? 1 : 0;
}

//! @todo Do not use conditionals
uint8_t dcmotor_phy_read_encoder_phase_b(void) {
  return (ENCODER_B_PIN & ENCODER_B_PIN_MASK) ? 1 : 0;
}

void dcmotor_phy_pid_init(uint16_t pid_interval_msec) {
  uint16_t ocr_value = pid_interval_msec * OCR_ONE_MSEC;
  PID_TCCRA = PID_TCCRA_VALUE;
  PID_TCCRB = PID_TCCRB_VALUE;
  PID_OCRH = ocr_value >> 8;
  PID_OCRL = ocr_value & 0x00FF;
}

void dcmotor_phy_pid_start(void) {
  PID_TCNT = 0;
  PID_TIMSK |= PID_TIMSK_MASK;
}

void dcmotor_phy_pid_stop(void) {
  PID_TIMSK &= ~PID_TIMSK_MASK;
}
