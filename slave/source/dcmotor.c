// AVR Multi Motor Control
/*! \file slave/source/dcmotor.c
 * DC Motor Slave Module
 * \todo Use 16-bit PWM
 *
 * \author Paolo Lucchesi
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "dcmotor.h"
#include "dcmotor_pid.h"

#define OCR_ONE_MSEC 15.625
#define abs(x) ((x) > 0 ? (x) : -(x))

typedef uint8_t duty_cycle_t;

typedef enum _DIRECTION_E {
  DIR_CLOCKWISE, DIR_COUNTER_CLOCKWISE
} direction_t;


static int64_t motor_position;
static volatile dc_rpm_t speed_actual;
static dc_rpm_t speed_target;
static dc_rpm_t speed_next;


static inline duty_cycle_t rpm2pwm(dc_rpm_t rpm) {
  return 0xFF * rpm / DC_MOTOR_MAX_RPM_SPEED; // Non-inverting
}

static inline void dcmotor_set_direction(direction_t dir) {
  if (dir == DIR_CLOCKWISE)
    PORTD &= ~(1 << 4);
  else if (dir == DIR_COUNTER_CLOCKWISE)
    PORTD |= ~(1 << 4);
}

// Initialize DC motor handling -- Use PORTD6, i.e. OC0A, as PWM pin
void dcmotor_init(void) {
  // TODO: Clean those ugly constants
  dcmotor_pid_init(0.80, 0.05, 0.15, DC_SAMPLING_INTERVAL);

  // Configure PWM timer -- Fast PWM, 8 bits, non inverted
  DDRD |= 1 << 6;
  TCCR0A = (1 << WGM00) | (1 << COM0A1) | (1 << COM0A0);
  TCCR0B = (1 << WGM01) | (1 << CS00);
  OCR0A = rpm2pwm(0);

  // Configure sampling interval timer
  TCCR2A = 0;
  TCCR2B = (1 << WGM22) | (1 << CS20) | (1 << CS22);

  // Setup digital input pins for the DC motor encoder
  // Use pins PD7 (i.e. D7) for Encoder A and PB0 (i.e. D8) for Encoder B
  DDRD &= ~(1 << 7); PORTD |= 1 << 7;
  DDRB &= ~(1 << 0); PORTB |= 1 << 0;

  // Set clockwise rotation direction as default
  DDRD |= (1 << 4); // Use pin D6
  dcmotor_set_direction(DIR_CLOCKWISE);

  // Enable interrupt for one of the encoder pins (we use Encoder Phase A)
  // Pin PD7 is PCINT0
  PCICR |= 1 << PCIE0;
  PCMSK0 |= 1 << 0;
}

dc_rpm_t dcmotor_get(void) {
  return speed_actual;
}

void dcmotor_set(dc_rpm_t next) {
  speed_next = next;
}

void dcmotor_apply(void) {
  dcmotor_set_direction(
      speed_next > 0 ? DIR_CLOCKWISE : DIR_COUNTER_CLOCKWISE);
  OCR0A = rpm2pwm(abs(speed_next));
  speed_target = speed_next;
}


// Encoder ISR
ISR(PCINT0_vect) {
  uint8_t enc_a = PIND | (1 << 7);
  uint8_t enc_b = PINB | (1 << 0);
  if (enc_a != 0) motor_position += (enc_b == 1) ? 1 : -1;
}


void dcmotor_pid_start(void) {
  TCNT1 = 0;
  TIMSK1 |=  (1 << OCIE1A);
}

void dcmotor_pid_stop(void) {
  TIMSK1 &= ~(1 << OCIE1A);
}


static inline float compute_speed_from_position(int64_t position) {
  return position * 60000 /
    DC_ENC_SIGNALS_PER_ROUND / DC_SAMPLING_INTERVAL;
}

// PID sampling timer ISR
ISR(TIMER1_COMPA_vect) {
  float speed = compute_speed_from_position(motor_position);
  float u = dcmotor_pid_iterate(speed, speed_target);

  dcmotor_set_direction(u > 0 ? DIR_CLOCKWISE : DIR_COUNTER_CLOCKWISE);
  OCR0A = rpm2pwm(abs((dc_rpm_t) u));

  // Setup for next sample
  motor_position = 0;
  speed_actual = (dc_rpm_t) speed;
}
