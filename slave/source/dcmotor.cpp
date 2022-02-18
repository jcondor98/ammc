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

namespace Phy = PhysicalMotor;

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
  Phy::initializePid(DC_SAMPLING_INTERVAL);
  Phy::initializeEncoder();
  Phy::initializePWM();
  Phy::loadSpeed(0);
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

  Phy::loadSpeed(corrected_speed);
  actualSpeed = (dc_rpm_t) speed;
  position = 0;
}

void DCMotor::startPid(void) {
  Phy::startPid();
}

void DCMotor::stopPid(void) {
  Phy::stopPid();
}

void DCMotor::updatePosition(void) {
  uint8_t enc_a = Phy::readEncoderPhaseA();
  uint8_t enc_b = Phy::readEncoderPhaseB();
  if (enc_a) enc_b ? ++position : --position;
}
