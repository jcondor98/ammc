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


void PhysicalMotor::initializePWM(void) {
  PWM_DDR |= PWM_DDR_MASK;
  PWM_TCCRA = PWM_TCCRA_VALUE;
  PWM_TCCRB = PWM_TCCRB_VALUE;
  DIRECTION_IN1_DDR |=  DIRECTION_IN1_DDR_MASK;
  DIRECTION_IN2_DDR |=  DIRECTION_IN2_DDR_MASK;
}

static inline void move_forward(void) {
  DIRECTION_IN1_PORT |=  DIRECTION_IN1_PORT_MASK;
  DIRECTION_IN2_PORT &= ~DIRECTION_IN2_PORT_MASK;
}

static inline void move_backward(void) {
  DIRECTION_IN1_PORT &= ~DIRECTION_IN1_PORT_MASK;
  DIRECTION_IN2_PORT |=  DIRECTION_IN2_PORT_MASK;
}

static inline void power_off_motor(void) {
  DIRECTION_IN1_PORT &= ~DIRECTION_IN1_PORT_MASK;
  DIRECTION_IN2_PORT &= ~DIRECTION_IN2_PORT_MASK;
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

void PhysicalMotor::loadSpeed(dc_rpm_t speed) {
  duty_cycle_t duty_cycle = rpm2pwm(bound_speed(abs(speed)));
  if (speed > 0)
    move_forward();
  else if (speed < 0)
    move_backward();
  else power_off_motor();
  SPEED_REGISTER = duty_cycle;
}

void PhysicalMotor::loadSpeed(float speed) {
  loadSpeed((dc_rpm_t) speed);
}

void PhysicalMotor::initializeEncoder(void) {
  ENCODER_A_DDR  &= ~ENCODER_A_DDR_MASK;
  ENCODER_B_DDR  &= ~ENCODER_B_DDR_MASK;
  ENCODER_A_PORT |= ENCODER_A_PORT_MASK;
  ENCODER_B_PORT |= ENCODER_B_PORT_MASK;
  ENCODER_PCMSK  |= ENCODER_PCMSK_MASK;
  ENCODER_PCICR  |= ENCODER_PCICR_MASK;
}

//! @todo Do not use conditionals
uint8_t PhysicalMotor::readEncoderPhaseA(void) {
  return (ENCODER_A_PIN & ENCODER_A_PIN_MASK) ? 1 : 0;
}

//! @todo Do not use conditionals
uint8_t PhysicalMotor::readEncoderPhaseB(void) {
  return (ENCODER_B_PIN & ENCODER_B_PIN_MASK) ? 1 : 0;
}

void PhysicalMotor::initializePid(uint16_t intervalInMillis) {
  uint16_t ocrValue = intervalInMillis * OCR_ONE_MSEC;
  PID_TCCRA = PID_TCCRA_VALUE;
  PID_TCCRB = PID_TCCRB_VALUE;
  PID_OCRH = ocrValue >> 8;
  PID_OCRL = ocrValue & 0x00FF;
}

void PhysicalMotor::startPid(void) {
  PID_TCNT = 0;
  PID_TIMSK |= PID_TIMSK_MASK;
}

void PhysicalMotor::stopPid(void) {
  PID_TIMSK &= ~PID_TIMSK_MASK;
}
