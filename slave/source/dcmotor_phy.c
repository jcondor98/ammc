// AVR Multi Motor Control
/*! \file slave/source/dcmotor_phy.c
 * DC Motor Slave Hardware Abstraction Layer
 *
 * Used I/O pins with ATMega2560 are:
 * D53 (PORTB0/PCINT0) -> Encoder Phase A
 * D52 (PORTB1/PCINT1) -> Encoder Phase B
 * D13 (PORTD7/OCR0A)  -> PWM output
 * D4  (PORTG5/OCR0B)  -> Direction switch
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>
#include "common/dcmotor.h"
#include "dcmotor_phy.h"
#include "dcmotor_phy_params.h"


#define OCR_ONE_MSEC 15.625
#define abs(x) ((x) > 0 ? (x) : -(x))

typedef uint8_t duty_cycle_t;

typedef enum _DIRECTION_E {
  DIR_CLOCKWISE, DIR_COUNTER_CLOCKWISE
} direction_t;


static inline duty_cycle_t rpm2pwm(dc_rpm_t rpm) {
  return 0xFF * rpm / DC_MOTOR_MAX_RPM_SPEED;
}

static inline void set_direction(direction_t dir) {
  if (dir == DIR_CLOCKWISE)
    DIRECTION_PORT &= ~DIRECTION_PORT_MASK;
  else if (dir == DIR_COUNTER_CLOCKWISE)
    DIRECTION_PORT |= DIRECTION_PORT_MASK;
}

void dcmotor_phy_encoder_init(void) {
  ENCODER_A_PORT |= ENCODER_A_PORT_MASK;
  ENCODER_B_PORT |= ENCODER_B_PORT_MASK;
  ENCODER_A_DDR &= ~ENCODER_A_DDR_MASK;
  ENCODER_B_DDR &= ~ENCODER_B_DDR_MASK;
  ENCODER_PCMSK |= ENCODER_PCMSK_MASK;
  ENCODER_PCICR |= ENCODER_PCICR_MASK;
}

void dcmotor_phy_pwm_init(void) {
  PWM_DDR |= PWM_DDR_MASK;
  PWM_TCCRA = PWM_TCCRA_VALUE;
  PWM_TCCRB = PWM_TCCRB_VALUE;
}

void dcmotor_phy_sampling_timer_init(void) {
  SAMPLING_TCCRA = SAMPLING_TCCRA_VALUE;
  SAMPLING_TCCRB = SAMPLING_TCCRB_VALUE;
}

void dcmotor_phy_load_speed_float(float _speed) {
  dc_rpm_t speed = (dc_rpm_t) _speed;
  set_direction(speed > 0 ? DIR_CLOCKWISE : DIR_COUNTER_CLOCKWISE);
  SPEED_REGISTER = rpm2pwm(abs(speed));
}

uint8_t dcmotor_phy_read_encoder_phase_a(void) {
  return ENCODER_A_PIN | ENCODER_A_PIN_MASK;
}

uint8_t dcmotor_phy_read_encoder_phase_b(void) {
  return ENCODER_B_PIN | ENCODER_B_PIN_MASK;
}

void dcmotor_pid_start(void) {
  PID_TCNT = 0;
  PID_TIMSK |= PID_TIMSK_MASK;
}

void dcmotor_pid_stop(void) {
  PID_TIMSK &= PID_TIMSK_MASK;
}
