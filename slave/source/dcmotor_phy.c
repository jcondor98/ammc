// AVR Multi Motor Control
/*! \file slave/source/dcmotor_phy.c
 * DC Motor Slave Hardware Abstraction Layer
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>
#include "common/dcmotor.h"
#include "dcmotor_phy.h"

#define OCR_ONE_MSEC 15.625
#define abs(x) ((x) > 0 ? (x) : -(x))

typedef uint8_t duty_cycle_t;

typedef enum _DIRECTION_E {
  DIR_CLOCKWISE, DIR_COUNTER_CLOCKWISE
} direction_t;


static inline duty_cycle_t rpm2pwm(dc_rpm_t rpm) {
  return 0xFF * rpm / DC_MOTOR_MAX_RPM_SPEED;
}


// TODO: Change this
static inline void set_direction(direction_t dir) {
  if (dir == DIR_CLOCKWISE)
    PORTD &= ~(1 << 4);
  else if (dir == DIR_COUNTER_CLOCKWISE)
    PORTD |= 1 << 4;
}

// Enable interrupt for one of the encoder pins (we use Encoder Phase A)
// Pin PD7 is PCINT0
void dcmotor_phy_encoder_init(void) {
  DDRD &= ~(1 << 7);
  DDRB &= ~(1 << 0);
  PORTD |= 1 << 7;
  PORTB |= 1 << 0;
  PCICR |= 1 << PCIE0;
  PCMSK0 |= 1 << 0;
}

void dcmotor_phy_pwm_init(void) {
  DDRD |= 1 << 6;
  TCCR0A = (1 << WGM00) | (1 << COM0A1) | (1 << COM0A0);
  TCCR0B = (1 << WGM01) | (1 << CS00);
}

void dcmotor_phy_sampling_timer_init(void) {
  TCCR2A = 0;
  TCCR2B = (1 << WGM22) | (1 << CS20) | (1 << CS22);
}

void dcmotor_phy_load_speed_float(float _speed) {
  dc_rpm_t speed = (dc_rpm_t) _speed;
  set_direction(speed > 0 ? DIR_CLOCKWISE : DIR_COUNTER_CLOCKWISE);
  OCR0A = rpm2pwm(abs(speed));
}

uint8_t dcmotor_phy_read_encoder_phase_a(void) {
  return PIND | (1 << 7);
}

uint8_t dcmotor_phy_read_encoder_phase_b(void) {
  return PINB | (1 << 0);
}

void dcmotor_pid_start(void) {
  TCNT1 = 0;
  TIMSK1 |=  (1 << OCIE1A);
}

void dcmotor_pid_stop(void) {
  TIMSK1 &= ~(1 << OCIE1A);
}
