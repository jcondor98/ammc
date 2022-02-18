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

// TODO: Make these dynamic and configurable
#define DCMOTOR_PID_KP 1.5
#define DCMOTOR_PID_KI 3.7
#define DCMOTOR_PID_KD 0.0

#define max(x,y) ((x) > (y) ? (x) : (y))
#define min(x,y) ((x) < (y) ? (x) : (y))

// DC motor instance
static DCMotor motor;


// Encoder ISR
ISR(DCMOTOR_PHY_ENCODER_ISR) {
  motor.updatePosition();
}

// PID sampling timer ISR
ISR(DCMOTOR_PHY_PID_ISR) {
  motor.adjustActualSpeed();
}

DCMotor::DCMotor(void)
  : pid({
      DCMOTOR_PID_KP,
      DCMOTOR_PID_KI,
      DCMOTOR_PID_KD,
      DC_SAMPLING_INTERVAL
    }),
    position(0),
    actualSpeed(0),
    targetSpeed(0),
    nextSpeed(0) {
  dcmotor_phy_pid_init(DC_SAMPLING_INTERVAL);
  dcmotor_phy_encoder_init();
  dcmotor_phy_pwm_init();
  dcmotor_phy_load_speed(0);
}

void DCMotor::setTargetSpeed(dc_rpm_t next) {
  nextSpeed = computeLimitedSpeed(next);
}

dc_rpm_t DCMotor::computeLimitedSpeed(dc_rpm_t speed) {
  return (speed >= 0)
    ? min(speed, DC_MOTOR_MAX_RPM_SPEED)
    : max(speed, DC_MOTOR_MAX_RPM_SPEED);
}

void DCMotor::applyTargetSpeed(void) {
  targetSpeed = nextSpeed;
}

dc_rpm_t DCMotor::getActualSpeed(void) {
  return actualSpeed;
}

dc_rpm_t DCMotor::sampleActualSpeed(void) {
  return ((double) position) / DC_ENC_SIGNALS_PER_ROUND
    * 60000 / DC_SAMPLING_INTERVAL;
}

void DCMotor::adjustActualSpeed(void) {
  float speed = sampleActualSpeed();
  float corrected_speed = pid.correct(speed, targetSpeed);

  dcmotor_phy_load_speed_float(corrected_speed);
  actualSpeed = (dc_rpm_t) speed;
  position = 0;
}

void DCMotor::startPid(void) {
  dcmotor_phy_pid_start();
}

void DCMotor::stopPid(void) {
  dcmotor_phy_pid_stop();
}

void DCMotor::updatePosition(void) {
  uint8_t enc_a = dcmotor_phy_read_encoder_phase_a();
  uint8_t enc_b = dcmotor_phy_read_encoder_phase_b();
  if (enc_a) enc_b ? ++position : --position;
}
