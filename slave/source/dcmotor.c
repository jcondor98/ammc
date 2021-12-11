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

#define OCR_ONE_MSEC 15.625
#define abs(x) ((x) > 0 ? (x) : -(x))

//! Type to properly store Rounds Per Minutes
typedef uint8_t duty_cycle_t;

//! Type defining the DC motor rotation direction
typedef enum _DC_DIRECTION_E {
  DIR_CLOCKWISE, DIR_COUNTER_CLOCKWISE
} dc_direction_t;


// DC motor RPM variables
static int64_t dc_motor_position;   // Raw position measured via encoder
static volatile dc_rpm_t rpm_actual; // Actual RPM measured via encoder
static dc_rpm_t rpm_target; // DC motor current target RPM
static dc_rpm_t rpm_next;   // Next target RPM to apply

// Rotation direction
dc_direction_t dc_motor_direction;

// Proportional Integral Derivative controller variables
static float pid_kp = 0.80; // Proportional weight
static float pid_ki = 0.05; // Integral weight
static float pid_kd = 0.15; // Derivative weight
static float pid_err_int;   // Integral error value
static float pid_err_prev;  // Previous error, used to compute Derivative
static uint8_t pid_ongoing; // Is the PID sampling daemon ongoing?


/*!
 * Convert RPM in a proper PWM duty cycle value
 *
 * @param rpm A speed value given in RPM
 */
static inline duty_cycle_t rpm2pwm(dc_rpm_t rpm) {
  return 0xFF * rpm / DC_MOTOR_MAX_RPM; // Non-inverting
}

/*!
 * Internally set rotation direction
 * \todo: Implement direction set
 */
static inline void dcmotor_set_direction(dc_direction_t dir) {
  if (dir == DIR_CLOCKWISE)
    PORTD &= ~(1 << 4);
  else if (dir == DIR_COUNTER_CLOCKWISE)
    PORTD |= ~(1 << 4);
}

// Initialize DC motor handling -- Use PORTD6, i.e. OC0A, as PWM pin
void dcmotor_init(void) {
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

// Get the last sampled speed, internally stored
dc_rpm_t dcmotor_get(void) { return rpm_actual; }

// Set a new speed (without applying it)
void dcmotor_set(dc_rpm_t next) {
  rpm_next = next;
}

// Apply the previously given RPM value
void dcmotor_apply(void) {
  dcmotor_set_direction(rpm_next > 0 ? DIR_CLOCKWISE : DIR_COUNTER_CLOCKWISE);
  OCR0A = rpm2pwm(abs(rpm_next));
  rpm_target = rpm_next;
}

// Start the DC motor PID sampling timer
void dcmotor_pid_start(void) {
  TCNT1 = 0;
  TIMSK1 |=  (1 << OCIE1A);
  pid_ongoing = 1;
}

// Stop the DC motor PID sampling timer
void dcmotor_pid_stop(void) {
  TIMSK1 &= ~(1 << OCIE1A);
  pid_ongoing = 0;
}


// Encoder ISR
ISR(PCINT0_vect) {
  uint8_t enc_a = PIND | (1 << 7);
  uint8_t enc_b = PINB | (1 << 0);
  if (enc_a != 0) dc_motor_position += (enc_b == 1) ? 1 : -1;
}


// PID sampling timer ISR
ISR(TIMER2_COMPA_vect) {
  // Finish actual RPM sampling
  float rpm = dc_motor_position * 60000 /
    DC_ENC_SIGNALS_PER_ROUND / DC_SAMPLING_INTERVAL;

  // Proportional factor (i.e. current error)
  float e_rpm = rpm - rpm_target;

  // Integral factor
  pid_err_int += e_rpm * DC_SAMPLING_INTERVAL / 60000;

  // Derivative factor
  float e_rpm_der = (e_rpm - pid_err_prev) * 60000 / DC_SAMPLING_INTERVAL;

  // Compute and apply PID corrected value
  float u = pid_kp * e_rpm + pid_ki * pid_err_int + pid_kd + e_rpm_der;
  dcmotor_set_direction(u > 0 ? DIR_CLOCKWISE : DIR_COUNTER_CLOCKWISE);
  OCR0A = rpm2pwm(abs((dc_rpm_t) u));

  // Setup for next sample
  dc_motor_position = 0;
  rpm_actual = (dc_rpm_t) rpm;
  pid_err_prev = e_rpm;
}
